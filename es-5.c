#include <math.h>
#include <spa/param/latency-utils.h>
#include <pipewire/pipewire.h>

#define ES_5_RANGE 128
#define GATE_LOW_EDGE 0.1

struct data;

struct port {
  struct data *data;
};

struct data {
  struct pw_main_loop *loop;
  struct pw_filter *filter;
  struct port *in_ports[8];
  struct port *out_port;
};

static void on_process(void *userdata, struct spa_io_position *position) {
  struct data *data = userdata;
  float *in, *out;
  uint32_t i, n_samples = position->clock.duration;
  float signal = 0.5f;
  
  // Read Inputs 1-7
  out = pw_filter_get_dsp_buffer(data->out_port, n_samples);
  for (int input = 0; input < 7; input++) {
    in = pw_filter_get_dsp_buffer(data->in_ports[input], n_samples);
    if (in == NULL || out == NULL) return;
    if(*in > GATE_LOW_EDGE) {
      signal += powf(2, input);
    }
  }

  // Read Input 8
  in = pw_filter_get_dsp_buffer(data->in_ports[7], n_samples);
  if (in == NULL || out == NULL) return; 
  if(*in > GATE_LOW_EDGE) {
    signal = -ES_5_RANGE+signal;
  }

  // Convert signal to expect output scale
  signal = signal / ES_5_RANGE;

  // Send to output
  for (i = 0; i < n_samples; i++) {
    *out++ = signal;
  }
}

static void do_quit(void *userdata, int signal_number) {
  struct data *data = userdata;
  pw_main_loop_quit(data->loop);
}

static const struct pw_filter_events filter_events = {
	PW_VERSION_FILTER_EVENTS,
	.process = on_process,
};

struct port* add_port(char* name, struct data data) {
  return  pw_filter_add_port(data.filter,
			     PW_DIRECTION_INPUT,
			     PW_FILTER_PORT_FLAG_MAP_BUFFERS,
			     sizeof(struct port),
			     pw_properties_new(
					       PW_KEY_FORMAT_DSP, "32 bit float mono audio",
					       PW_KEY_PORT_NAME, name,
					       NULL),
			     NULL, 0);
}

int main(int argc, char *argv[]) {
  struct data data = { 0, };
  const struct spa_pod *params[1];
  uint8_t buffer[1024];
  struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
  
  pw_init(&argc, &argv);

  data.loop = pw_main_loop_new(NULL);

  pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGINT, do_quit, &data);
  pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGTERM, do_quit, &data);

  data.filter = pw_filter_new_simple(
				     pw_main_loop_get_loop(data.loop),
				     "ES-5",
				     pw_properties_new(
						       PW_KEY_MEDIA_TYPE, "Audio",
						       PW_KEY_MEDIA_CATEGORY, "Filter",
						       PW_KEY_MEDIA_ROLE, "DSP",
						       NULL),
				     &filter_events,
				     &data);

  data.in_ports[0] = add_port("input_1", data);
  data.in_ports[1] = add_port("input_2", data);
  data.in_ports[2] = add_port("input_3", data);
  data.in_ports[3] = add_port("input_4", data);
  data.in_ports[4] = add_port("input_5", data);
  data.in_ports[5] = add_port("input_6", data);
  data.in_ports[6] = add_port("input_7", data);
  data.in_ports[7] = add_port("input_8", data);

  data.out_port = pw_filter_add_port(data.filter,
				     PW_DIRECTION_OUTPUT,
				     PW_FILTER_PORT_FLAG_MAP_BUFFERS,
				     sizeof(struct port),
				     pw_properties_new(
						       PW_KEY_FORMAT_DSP, "32 bit float mono audio",
						       PW_KEY_PORT_NAME, "output",
						       NULL),
				     NULL, 0);

  params[0] = spa_process_latency_build(&b,
					SPA_PARAM_ProcessLatency,
					&SPA_PROCESS_LATENCY_INFO_INIT(
								       .ns = 10 * SPA_NSEC_PER_MSEC
								       ));

  if (pw_filter_connect(data.filter,
			PW_FILTER_FLAG_RT_PROCESS,
			params, 1) < 0) {
    fprintf(stderr, "can't connect\n");
    return -1;
  }

  pw_main_loop_run(data.loop);

  pw_filter_destroy(data.filter);
  pw_main_loop_destroy(data.loop);
  return 0;
}
