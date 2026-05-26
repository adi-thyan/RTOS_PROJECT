// ================== LIBRARIES ==================

#include <WiFi.h>

// ================== PIN CONFIG ==================

#define LDR_PIN      32
#define LASER_PIN    33

#define PIR_PIN      26

#define MQ2_PIN      34

#define BUZZER_PIN   25

#define PIR_LED      4
#define GAS_LED      14

// ================== VARIABLES ==================

volatile bool intrusionDetected = false;
volatile bool smokeDetected = false;
volatile bool motionDetected = false;

int gasThreshold = 500;

// ======================================================
// STATE CHANGE VARIABLES
// ======================================================

bool intrusionSent = false;
bool smokeSent = false;
bool motionSent = false;

bool enemySent = false;
bool attackSent = false;

// ======================================================
// FUNCTION DECLARATIONS
// ======================================================

void intrusionTask(void *pvParameters);
void smokeTask(void *pvParameters);
void motionTask(void *pvParameters);
void scenarioTask(void *pvParameters);

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  // ================== INPUTS ==================

  pinMode(LDR_PIN, INPUT);

  pinMode(PIR_PIN, INPUT);

  pinMode(MQ2_PIN, INPUT);

  // ================== OUTPUTS ==================

  pinMode(LASER_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(PIR_LED, OUTPUT);

  pinMode(GAS_LED, OUTPUT);

  // ================== LASER ON ==================

  digitalWrite(LASER_PIN, HIGH);

  Serial.println("System Starting");

  // ======================================================
  // PIR STABILIZATION
  // ======================================================

  delay(30000);

  Serial.println("System Ready");

  // ======================================================
  // TASK CREATION
  // ======================================================

  xTaskCreatePinnedToCore(
    intrusionTask,
    "Intrusion",
    4096,
    NULL,
    3,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    smokeTask,
    "Smoke",
    4096,
    NULL,
    2,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    motionTask,
    "Motion",
    4096,
    NULL,
    1,
    NULL,
    1
  );

  xTaskCreatePinnedToCore(
    scenarioTask,
    "Scenario",
    4096,
    NULL,
    1,
    NULL,
    0
  );
}

// ======================================================
// INTRUSION TASK
// ======================================================

void intrusionTask(void *pvParameters) {

  while (1) {

    if (digitalRead(LDR_PIN) == HIGH) {

      intrusionDetected = true;

      digitalWrite(BUZZER_PIN, HIGH);

      if (!intrusionSent) {

        Serial.println("INTRUSION DETECTED");

        intrusionSent = true;
      }
    }
    else {

      intrusionDetected = false;

      intrusionSent = false;

      digitalWrite(BUZZER_PIN, LOW);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);

    taskYIELD();
  }
}

// ======================================================
// SMOKE TASK
// ======================================================

void smokeTask(void *pvParameters) {

  while (1) {

    int gasValue = analogRead(MQ2_PIN);

    if (gasValue > gasThreshold) {

      smokeDetected = true;

      digitalWrite(GAS_LED, HIGH);

      if (!smokeSent) {

        Serial.println("SMOKE DETECTED");

        smokeSent = true;
      }
    }
    else {

      smokeDetected = false;

      smokeSent = false;

      digitalWrite(GAS_LED, LOW);
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);

    taskYIELD();
  }
}

// ======================================================
// MOTION TASK
// ======================================================

void motionTask(void *pvParameters) {

  while (1) {

    if (digitalRead(PIR_PIN) == HIGH) {

      motionDetected = true;

      digitalWrite(PIR_LED, HIGH);

      if (!motionSent) {

        Serial.println("MOTION DETECTED");

        motionSent = true;
      }
    }
    else {

      motionDetected = false;

      motionSent = false;

      digitalWrite(PIR_LED, LOW);
    }

    vTaskDelay(250 / portTICK_PERIOD_MS);

    taskYIELD();
  }
}

// ======================================================
// SCENARIO TASK
// ======================================================

void scenarioTask(void *pvParameters) {

  while (1) {

    // ==================================================
    // CONFIRMED ENEMY DETECTION
    // ==================================================

    if (intrusionDetected && motionDetected) {

      if (!enemySent) {

        Serial.println("CONFIRMED ENEMY DETECTED");

        enemySent = true;
      }
    }
    else {

      enemySent = false;
    }

    // ==================================================
    // POSSIBLE BORDER ATTACK
    // ==================================================

    if (smokeDetected && motionDetected) {

      if (!attackSent) {

        Serial.println("POSSIBLE BORDER ATTACK DETECTED");

        attackSent = true;
      }
    }
    else {

      attackSent = false;
    }

    vTaskDelay(300 / portTICK_PERIOD_MS);

    taskYIELD();
  }
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  delay(10);
}