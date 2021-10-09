#include <pipewire/pipewire.h>

struct data {
  struct pw_main_loop *loop;
  struct pw_stream *stream;
};

static void on_process(void *userdata) {}

static const struct pw_stream_events stream_events = {
  PW_VERSION_STREAM_EVENTS,
  .process = on_process,
};

int main(int argc, char *argv[]) {
  struct data data = { 0, };
  pw_init(&argc, &argv);

  data.loop = pw_main_loop_new(NULL);

  data.stream = pw_stream_new_simple(
				     pw_main_loop_get_loop(data.loop),
				     "audio-src",
				     pw_properties_new(
						       PW_KEY_MEDIA_TYPE, "Audio",
						       PW_KEY_MEDIA_CATEGORY, "Duplex",
						       PW_KEY_MEDIA_ROLE, "DSP",
						       NULL),
				     &stream_events,
				     &data);


  pw_stream_destroy(data.stream);
  pw_main_loop_destroy(data.loop);
  return 0;
}
