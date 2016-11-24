/*
  Doorbot
  For South London Makerspace
  
  To read cards serial numbers 
  Query remote server for access rights.
  Open close shutter.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <inttypes.h>
#include <MFRC522.h>
#include <Metro.h>
#include <utility/w5100.h>

#define lockRelay 6
#define doorOpen  5
#define doorMon   4
#define ethSS     10
#define rfidRST   8
#define rfidSS    9
#define rfidMOSI  11
#define rfidMISO  12
#define rfidSCK   13

#define doorReleaseDelay 100
#define deviceID F( "door" )

// Ethernet configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// numeric IP for server node
IPAddress nodebotServer( 10, 0, 1, 2 );
uint16_t port = 1234;

// Initialize the Ethernet client library
EthernetClient client;
EthernetServer server = EthernetServer( 1234 );

// Initialsie RFID reader
MFRC522 mfrc522( rfidSS, rfidRST );	

String previous_card_number;        // Stores previously read card number for a time
boolean previous_card_valid;        // previously red card state valid true or false

int attempts = 0;

Metro rfidReadTimer = Metro( 100 ); 
Metro netTimeout = Metro( 1000 );
Metro cardTimer = Metro( 5000 );
Metro unlockTimer = Metro( 2000 );

char* masterCards[] = { "" };

void setup() {
  pinMode( ethSS, OUTPUT );
  pinMode( rfidSS, OUTPUT );
  pinMode( lockRelay, OUTPUT );
  pinMode( doorOpen, OUTPUT );
  pinMode( doorMon, INPUT );
  
  // Set default relay state  
  digitalWrite( lockRelay, LOW );
  digitalWrite( doorOpen, LOW );
 
  Serial.begin( 115200 );
  Serial.println( F( "Doorbot Started" ) );

  Serial.println( F( "RFID" ) );
  SPI.begin();
  EnableSPI_RFID();
  mfrc522.PCD_Init();
 
  Serial.println( F( "Ethernet" ) );
  EnableSPI_Ethernet();
  Ethernet.begin( mac );
  W5100.setRetransmissionTime( 0x07d0 ); // reduce ethernet connection timeout
  W5100.setRetransmissionCount( 3 );     // set ethernet connection retries
  
  Serial.println( Ethernet.localIP() );
}

void loop() {
  if ( rfidReadTimer.check() ) rfidRead();
  if ( cardTimer.check() ) clearCard();
  
  if ( Ethernet.maintain() > 0 )
    Serial.println( Ethernet.localIP() );
}
