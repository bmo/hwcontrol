#include "ringbuffer.h"
#include "stdio.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

void RingBufferU8_init(RingBufferU8* ring, uint8_t* storage, uint16_t size) {
  ring->storage = storage;
  ring->size = size;
  ring->end = ring->storage + ring->size;
  ring->read = ring->storage;
  ring->write = ring->storage;
  ring->available = 0;
}

uint16_t RingBufferU8_available(RingBufferU8* ring) {
  return ring->available;
}

uint16_t RingBufferU8_free(RingBufferU8* ring) {
  return ring->size - ring->available;
}

void RingBufferU8_clear(RingBufferU8* ring) {
  ring->read = ring->storage;
  ring->write = ring->storage;
  ring->available = 0;
}

uint8_t RingBufferU8_readByte(RingBufferU8* ring) {
  if (ring->available == 0) {
    return 0;
  }
  uint8_t ret = *ring->read++;
  ring->available--;
  if (ring->read >= ring->end) {
    ring->read = ring->storage;
  }
  return ret;
}

void RingBufferU8_read(RingBufferU8* ring, uint8_t* buffer, uint16_t size) {
  uint16_t i;

  // TODO can be optimized
  for (i = 0; i < size; i++) {
    buffer[i] = RingBufferU8_readByte(ring);
  }
}

void RingBufferU8_writeByte(RingBufferU8* ring, uint8_t b) {
  if (ring->available >= ring->size) {
    RingBufferU8_readByte(ring);
  }

  *ring->write = b;
  ring->write++;
  ring->available++;
  if (ring->write >= ring->end) {
    ring->write = ring->storage;
  }
}

// if terminator is supplied
uint8_t RingBufferU8_write(RingBufferU8* ring, const uint8_t* buffer, uint16_t size, uint8_t terminator) {
  uint16_t i;
  uint8_t saw_terminator = 0;

  // TODO can be optimized
  for (i = 0; i < size; i++) {
    if (terminator && terminator == buffer[i]) saw_terminator = 1;
    RingBufferU8_writeByte(ring, buffer[i]);
  }
  return saw_terminator;
}

uint16_t RingBufferU8_readLine(RingBufferU8* ring, uint8_t* buffer, uint16_t size) {
  return RingBufferU8_readUntil(ring, buffer, size, '\r');
}

uint16_t RingBufferU8_readUntil(RingBufferU8* ring, uint8_t* buffer, uint16_t size, uint8_t stopByte) {
  uint8_t b;
  uint16_t i;
  for (i = 0; i < MIN(ring->available, size - 1); i++) {
    b = RingBufferU8_peekn(ring, i);
    if (b == stopByte) {
      i++;
      RingBufferU8_read(ring, (uint8_t*) buffer, i);
      buffer[i] = '\0';
      return i;
    }
  }
  buffer[0] = '\0';
  return 0;
}

uint8_t RingBufferU8_peek(RingBufferU8* ring) {
  return RingBufferU8_peekn(ring, 0);
}

uint8_t RingBufferU8_peekn(RingBufferU8* ring, uint16_t i) {
  if (i >= ring->available) {
    return 0;
  }

  uint8_t* read = (uint8_t*)ring->read;
  uint8_t* p = read + i;
  if (p >= ring->end) {
    p -= ring->size;
  }
  return *p;
}

void byte_to_hex(uint8_t val, uint8_t *buffer) {
  *buffer = ((val >> 4) & 0x0f) | 0x30;
  if (*buffer > 0x39) *buffer+=7;

  buffer++;

  *buffer = (val & 0x0f) | 0x30;
  if (*buffer > 0x39) *buffer+=7;

}


/*
#define BUFFERSIZE 1024
uint8_t buffer[BUFFERSIZE];
uint8_t saw_terminator = 0;
char l_s[] = "This is a string;This is another string; This is yet another string; but not this one";
char *l = l_s;
RingBufferU8 rb1;

char cmd_buffer[BUFFERSIZE];

int main(int argv, char **argc) {
  int len = 0;
  RingBufferU8_init(&rb1, buffer, BUFFERSIZE);
  printf("Initialized\n");
  saw_terminator = RingBufferU8_write(&rb1, (const unsigned char *)l, sizeof(l_s), ';');
  printf("Stored is %d, saw terminator %d\n",RingBufferU8_available(&rb1), saw_terminator);
  while ( (len = RingBufferU8_readUntil(&rb1, cmd_buffer, BUFFERSIZE, ';')) ) {
      cmd_buffer[len] = 0;
      printf("Read %s\n",cmd_buffer);
    }
  printf("Available %d\n",RingBufferU8_available(&rb1));
  cmd_buffer[2]=0;
  for (int i=0; i<=255;i++) {
    byte_to_hex(i,(uint8_t *)cmd_buffer+(3*i));
    cmd_buffer[3*i+2]=0x20;
  }
  cmd_buffer[256*3+1]=0;
  printf("Converted %s\n",cmd_buffer);
}
*/
