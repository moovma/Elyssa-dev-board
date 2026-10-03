/*
  Elyssa IMU - 03 Angles and orientation
  Tilt angles (pitch, roll) and which side of the board faces up.
  Turn the board in your hand and watch the values.
*/
const char *sideName(ElyssaOrientation o) {
  switch (o) {
    case ELYSSA_FACE_UP:   return "FACE UP";
    case ELYSSA_FACE_DOWN: return "FACE DOWN";
    case ELYSSA_X_UP:      return "X UP";
    case ELYSSA_X_DOWN:    return "X DOWN";
    case ELYSSA_Y_UP:      return "Y UP";
    case ELYSSA_Y_DOWN:    return "Y DOWN";
    default:               return "between two sides";
  }
}

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
}

void loop() {
  Serial.printf("pitch %6.1f  roll %6.1f  %s\n", elyssa_imu_pitch(), elyssa_imu_roll(),
                sideName(elyssa_imu_orientation()));
  delay(200);
}
