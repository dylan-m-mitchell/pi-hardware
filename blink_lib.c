#include <time.h>
#include <gpiod.h>
#include <stdio.h>

void wait(unsigned int sec) {
	unsigned int ret_time = time(0) + sec;
	while (time(0) < ret_time);
}

int main(void) {
	const char *path = "/dev/gpiochip0";
	const unsigned int offset = 16;

	struct gpiod_chip *chip = gpiod_chip_open(path);
	if (!chip) { perror("chip_open error"); return -1; }

	struct gpiod_line_settings *settings = gpiod_line_settings_new();
	gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
	gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);

	struct gpiod_line_config *line_cfg = gpiod_line_config_new();
	gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings);

	struct gpiod_request_config *req_cfg = gpiod_request_config_new(); 
	gpiod_request_config_set_consumer(req_cfg, "demo");

	struct gpiod_line_request *req = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
	if (!req) { perror("request lines"); return -1; }

	while (time(0) < (time(0) + 30)) {
		gpiod_line_request_set_value(req, offset, GPIOD_LINE_VALUE_ACTIVE);
		wait(1);	
		gpiod_line_request_set_value(req, offset, GPIOD_LINE_VALUE_INACTIVE);
		wait(1);	
	}

	gpiod_line_request_release(req);
	gpiod_request_config_free(req_cfg);
	gpiod_line_config_free(line_cfg);
	gpiod_line_settings_free(settings);
	gpiod_chip_close(chip);

	return 0;
}
