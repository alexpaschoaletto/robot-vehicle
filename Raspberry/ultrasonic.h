#include "pegasoboard.h"

#ifndef ultrasonic_h
#define ultrasonic_h

int average(int array[], int size){
  int sum = 0;
  for(int i = 0; i < size; i++){
    sum += array[i];
  }
  return (sum / size);
}

bool hasInvalidReadings(int array[], int size){
  for(int i = 0; i < size; i++){
    if(array[i] <= 0) return true;
  }
  return false;
}

long sample(ultrasonicSensor_t *sensor){
  digitalWrite(sensor->trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensor->trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensor->trigPin, LOW);
  return (pulseIn(sensor->echoPin, HIGH) >> 1);
}

void update(ultrasonicSensor_t *sensor){
  int distance = sample(sensor);
  if(distance > 0){
    sensor->readings[sensor->r] = distance;
    sensor->r = (sensor->r + 1) % SAMPLES;
  }
  if(hasInvalidReadings(sensor->readings, SAMPLES)) return;
  sensor->distance = average(sensor->readings, SAMPLES);
  sensor->next += sensor->period;
}

#endif