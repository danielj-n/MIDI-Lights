int new_data[3] = {};
bool new_data_on_off = false;

int PWML_start_new[] = {0, 0, 0, 0, 0, 0};
int PWML_time_new[] = {0, 0, 0, 0, 0, 0};
int PWML_end_new[] = {0, 0, 0, 0, 0, 0};
bool PWML_being_written[] = {false, false, false, false, false, false};
long PWML_began_at[] = {0, 0, 0, 0, 0, 0};
int PWML_start[] = {0, 0, 0, 0, 0, 0};
int PWML_time[] = {0, 0, 0, 0, 0, 0};
int PWML_end[] = {0, 0, 0, 0, 0, 0};

float time_constant = 0.00;
float BPM = 0.00;
float BPM_divider = 0.00;


int channels_to_pins[] = {3, 5, 6, 9, 10, 11};

bool new_data_available = false;

void setup() {
	Serial.begin(31250);
	for (int p = 0; p < 6; p++) {
	 pinMode(channels_to_pins[p], OUTPUT);
	} 
}

void loop() {
	if (Serial.available() > 0) {
		get_input_data();
	}
	if (new_data_available == true) {
		new_data_available = false;
		int channel = 0;
		if (new_data[0] < 144) {
			channel = new_data[0] - 128;
			new_data_on_off = false;
		}
		else {
			channel = new_data[0] - 144;
			new_data_on_off = true;
		}
		int note = new_data[1];
		int velocity = new_data[2];
		Serial.println(new_data[0]);
		Serial.println(note);
		Serial.println(velocity); 
		Serial.println("++++++");
		switch (note) {
			case 0 :
				if (new_data_on_off == true) {
					PWML_being_written[channel] = false;
					analogWrite(channels_to_pins[channel], velocity*2);
				}
				else {
					analogWrite(channels_to_pins[channel], 0);
				}
				break;
			case 1 :
				if (new_data_on_off == true) {
					PWML_end_new[channel] = velocity*2;
				}
				break;
			case 2 :
				if (new_data_on_off == true) {
					PWML_time_new[channel] = velocity;
				}
				break;
			case 3 :
				if (new_data_on_off == true) {
					PWML_start_new[channel] = velocity*2;
				}
				break;
			case 4 :
				if (new_data_on_off == true) {
					BPM_divider = velocity;
				}
				break;
			case 5 :
				if (new_data_on_off == true) {
					BPM = velocity;
				}
				break;
		}
	}
	for (int light = 0; light < 6; light++) {
		if (PWML_being_written[light] == true) {
			if (millis() < PWML_began_at[light]+PWML_time[light]*time_constant) {
				int brightness = map(millis(), PWML_began_at[light], PWML_began_at[light]+PWML_time[light]*time_constant, PWML_start[light], PWML_end[light]);
				if (brightness < 5) {
					brightness = 0;
				}
				analogWrite(channels_to_pins[light], brightness);
			}
			else {
				PWML_being_written[light] = false;	
			}
		}
		else {
			if (PWML_start_new[light] != 0 && PWML_time_new[light] != 0 && PWML_end_new[light] != 0) {
				PWML_being_written[light] = true;
				PWML_began_at[light] = millis();
				PWML_start[light] = PWML_start_new[light];
				PWML_time[light] = PWML_time_new[light];
				PWML_end[light] = PWML_end_new[light];
				PWML_start_new[light] = 0;
				PWML_time_new[light] = 0;
				PWML_end_new[light] = 0;
			}
		}
	}
	if (BPM != 0 && BPM_divider != 0) {
		time_constant = (60/BPM/BPM_divider)*1000;
		BPM = 0.00;
		BPM_divider = 0.00;
	}
}


void get_input_data() {
	for (int i = 0; i < 3; ++i) {
		while (Serial.available() <= 0) {
		}
		new_data[i] = Serial.read();
		if (new_data[i] == 255) {
			--i;
		}
	}
	new_data_available = true;
}
