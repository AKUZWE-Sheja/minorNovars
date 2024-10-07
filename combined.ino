#include <SoftwareSerial.h>
#include <Wire.h>

#define RE 8
#define DE 7

const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];

SoftwareSerial mod(2, 3);

void setup() {
    Serial.begin(9600);
    mod.begin(9600);
    pinMode(RE, OUTPUT);
    pinMode(DE, OUTPUT);
    delay(500);
}

void loop() {
    byte val1, val2, val3;

    val1 = nitrogen();
    delay(250);
    val2 = phosphorous();
    delay(250);
    val3 = potassium();
    delay(250);

    // Convert to ppm (same as mg/kg)
    Serial.print("Nitrogen: ");
    Serial.print(val1);
    Serial.println(" ppm");

    Serial.print("Phosphorous: ");
    Serial.print(val2);
    Serial.println(" ppm");

    Serial.print("Potassium: ");
    Serial.print(val3);
    Serial.println(" ppm");

    // Convert to percentage
    Serial.print("Nitrogen: ");
    Serial.print(val1 / 10000.0, 6);  // Print as percentage with six decimal places
    Serial.println(" %");

    Serial.print("Phosphorous: ");
    Serial.print(val2 / 10000.0, 6);
    Serial.println(" %");

    Serial.print("Potassium: ");
    Serial.print(val3 / 10000.0, 6);
    Serial.println(" %");

    delay(2000);
}


byte nitrogen() {
    return readSensor(nitro, sizeof(nitro), "Nitrogen");
}

byte phosphorous() {
    return readSensor(phos, sizeof(phos), "Phosphorous");
}

byte potassium() {
    return readSensor(pota, sizeof(pota), "Potassium");
}

byte readSensor(const byte* command, byte commandSize, const char* nutrientName) {
    digitalWrite(DE, HIGH);
    digitalWrite(RE, HIGH);
    delay(20);

    mod.write(command, commandSize);

    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    delay(50); // Give time for the sensor to respond

    int responseBytes = mod.available();
    Serial.print(nutrientName);
    Serial.print(" - Response bytes available: ");
    Serial.println(responseBytes);

    if (responseBytes == 7) { // Expecting 7 bytes (Modbus response)
        for (byte i = 0; i < responseBytes; i++) {
            values[i] = mod.read();
            Serial.print(values[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        // Check if the response starts with the correct Modbus address and function code
        if (values[0] == 0x01 && values[1] == 0x03) {
            // Extract the two bytes of the data (values[3] and values[4])
            byte highByte = values[3];  // Data high byte
            byte lowByte = values[4];   // Data low byte
            int result = (highByte << 8) | lowByte; // Combine high and low bytes
            return result;
        } else {
            Serial.println("Invalid response from sensor.");
        }
    } else {
        Serial.println("Incorrect number of response bytes or no response.");
    }

    if (responseBytes > 0) {
    Serial.print("Raw Response: ");
    for (int i = 0; i < responseBytes; i++) {
        Serial.print(Serial.read(), HEX);
        Serial.print(" ");
    }
    Serial.println();
}

    return 0; // Return 0 if there's an error
}


// #include <SoftwareSerial.h>
// #include <Wire.h>

// #define RE D1 // RE pin mapped to GPIO5 (D1)
// #define DE D2 // DE pin mapped to GPIO4 (D2)

// const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
// const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
// const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

// byte values[11];

// // Use GPIO pins for SoftwareSerial
// SoftwareSerial mod(D5, D6); // D5 (GPIO 14) as RX, D6 (GPIO 12) as TX

// void setup() {
//     Serial.begin(9600);
//     mod.begin(9600);
//     pinMode(RE, OUTPUT);
//     pinMode(DE, OUTPUT);
//     delay(500);
// }

// void loop() {
//     byte val1, val2, val3;

//     val1 = nitrogen();
//     delay(250);
//     val2 = phosphorous();
//     delay(250);
//     val3 = potassium();
//     delay(250);

//     // Print in ppm (mg/kg)
//     Serial.print("Nitrogen: ");
//     Serial.print(val1);
//     Serial.println(" ppm");

//     Serial.print("Phosphorous: ");
//     Serial.print(val2);
//     Serial.println(" ppm");

//     Serial.print("Potassium: ");
//     Serial.print(val3);
//     Serial.println(" ppm");

//     // Convert to percentage
//     Serial.print("Nitrogen: ");
//     Serial.print(val1 / 10000.0, 6);  // Print as percentage
//     Serial.println(" %");

//     Serial.print("Phosphorous: ");
//     Serial.print(val2 / 10000.0, 6);
//     Serial.println(" %");

//     Serial.print("Potassium: ");
//     Serial.print(val3 / 10000.0, 6);
//     Serial.println(" %");

//     delay(2000);
// }

// byte nitrogen() {
//     return readSensor(nitro, sizeof(nitro), "Nitrogen");
// }

// byte phosphorous() {
//     return readSensor(phos, sizeof(phos), "Phosphorous");
// }

// byte potassium() {
//     return readSensor(pota, sizeof(pota), "Potassium");
// }

// byte readSensor(const byte* command, byte commandSize, const char* nutrientName) {
//     digitalWrite(DE, HIGH); // Enable Modbus TX
//     digitalWrite(RE, HIGH); // Enable Modbus TX
//     delay(20);

//     mod.write(command, commandSize); // Send command to sensor

//     digitalWrite(DE, LOW); // Disable Modbus TX (to receive)
//     digitalWrite(RE, LOW); // Disable Modbus TX (to receive)
//     delay(50); // Wait for response

//     int responseBytes = mod.available();
//     Serial.print(nutrientName);
//     Serial.print(" - Response bytes available: ");
//     Serial.println(responseBytes);

//     if (responseBytes == 7) { // Expecting 7 bytes (Modbus response)
//         for (byte i = 0; i < responseBytes; i++) {
//             values[i] = mod.read();
//             Serial.print(values[i], HEX);
//             Serial.print(" ");
//         }
//         Serial.println();

//         if (values[0] == 0x01 && values[1] == 0x03) {
//             byte highByte = values[3];  // Data high byte
//             byte lowByte = values[4];   // Data low byte
//             int result = (highByte << 8) | lowByte; // Combine high and low bytes
//             return result;
//         } else {
//             Serial.println("Invalid response from sensor.");
//         }
//     } else {
//         Serial.println("Incorrect number of response bytes or no response.");
//     }

//     return 0; // Return 0 if there's an error
// }
