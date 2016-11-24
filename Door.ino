void releaseDoor() {
  if ( unlockTimer.check() ) {
    unlockTimer.reset();
    Serial.println( F( "Releasing Door" ) );
    digitalWrite( lockRelay, HIGH );
    delay( doorReleaseDelay );
    digitalWrite( lockRelay, LOW );
  }
}
