#if 0 // not used

#define SHIFT(a, s) ((s >= 0) ? (a >> s) : (a << (-s)))

#define HALT_FULL 0xB000

// create two buffers of 4kb each made of uint32_t
uint32_t buffer1[1024];
uint32_t buffer2[1024];
int activeIndex = 0;
int inactiveIndex = 0;

uint32_t* activeBuffer = buffer1;
uint32_t* inactiveBuffer = buffer2;

// double buffering 
bool done_reading = false;
bool done_showing = false;


uint32_t load_from_pc() {
  uint32_t word = activeBuffer[activeIndex];
  activeIndex++;
  if (activeIndex == 1024) {
    activeIndex = 0;
    done_reading = true;
  }
  return word;
}

void execute_instruction(uint32_t instruction) {
  int op_word1 = self.load_from_pc(memory);
  Instruction op = Dvg::instruction_from_word(op_word1);
  int op_word2 = op == Instruction::VCTR || op == Instruction::LABS ? self.load_from_pc(memory) : 0;

  switch (op) {
    case Instruction::VCTR: {
        bool ys = (0x400 & op_word1) != 0;
        int delta_y = 0x3FF & op_word1;
        int z = (0xF000 & op_word2) >> 12;
        bool xs = (0x400 & op_word2) != 0;
        int delta_x = 0x3FF & op_word2;
        int shift_bits = 9 - ((op_word1 & 0xF000) >> 12) + self.sf;
        int x = self.x + SHIFT(delta_x, shift_bits) * (xs ? -1 : 1);
        int y = self.y + SHIFT(delta_y, shift_bits) * (ys ? -1 : 1);
        self.line(x, y, z, canvas);
        break;
    }

    case Instruction::LABS: {
        bool ys = (0x400 & op_word1) != 0;
        int y = 0x3FF & op_word1;
        bool xs = (0x400 & op_word2) != 0;
        int x = 0x3FF & op_word2;
        int sf = (op_word2 & 0xF000) >> 12;
        self.sf = sf & 0x8 == 0 ? -sf : 16 - sf;
        self.y = ys ? 0 - ((y ^ 0x3FF) + 1) : y;
        self.x = xs ? 0 - ((x ^ 0x3FF) + 1) : x;
        break;
    }

    case Instruction::HALT: {
        memory->mapped_io.halt = 0;
        break;
    }
    case Instruction::JSRL: {
        if (self.sp > 3) {
            throw std::runtime_error("DVG stack overflow");
        }
        int addr = op_word1 & 0xFFF;
        self.stack[self.sp] = self.pc;
        self.sp += 1;
        self.pc = addr;
        break;
    }
    case Instruction::RTSL: {
        if (self.sp == 0) {
            throw std::runtime_error("DVG stack underflow");
        }
        self.sp -= 1;
        self.pc = self.stack[self.sp];
        break;
    }
    case Instruction::JMPL: {
        int a = op_word1 & 0xFFF;
        self.pc = a;
        break;
    }
    case Instruction::SVEC: {
      int sf = ((op_word1 & 0x800) >> 11) + ((op_word1 & 0x8) >> 2);
      bool ys = (op_word1 & 0x400) != 0;
      int delta_y = op_word1 & 0x300;
      bool xs = (op_word1 & 0x4) != 0;
      int delta_x = (op_word1 & 0x3) << 8;
      int z = (op_word1 & 0xF0) >> 4;

      int shift_bits = (7 - sf) + this->sf;
      int x = this->x + SHIFT(delta_x, shift_bits) * (xs ? -1 : 1);
      int y = this->y + SHIFT(delta_y, shift_bits) * (ys ? -1 : 1);
      this->line(x, y, z, canvas);
      break;
    }


}

void grab_instruction() {
  if(!done_reading && Serial.available()) {
    uint32_t command = Serial.read() | Serial.read() << 8 | Serial.read() << 16 | Serial.read() << 24;
    inactiveBuffer[inactiveIndex] = command;
    inactiveIndex++;
    if(command == HALT_FULL) {
      done_reading = true;
    }
  }
}

void swap_buffers() { 
  // swap active buffer and inactive buffer
  // reset both indices
  // reset done flags
  uint32_t* temp = activeBuffer;
  activeBuffer = inactiveBuffer;
  inactiveBuffer = temp;

  activeIndex = 0;
  inactiveIndex = 0;

  done_reading = false;
  done_showing = false;
}

void loop() {
  execute_instruction();
  
  grab_instruction();

  if(done_reading && done_showing) {
    swap_buffers();
  }
}




#endif