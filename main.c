#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEBUG false

#define HALT 0X00
#define SET  0X01
#define PUSH 0X02
#define POP  0X03
#define EQ   0X04
#define GT   0X05
#define JMP  0X06
#define JT   0X07
#define JF   0X08
#define ADD  0X09
#define MULT 0X0A
#define MOD  0X0B
#define AND  0X0C
#define OR   0X0D
#define NOT  0X0E
#define RMEM 0X0F
#define WMEM 0X10
#define CALL 0x11
#define RETN 0X12
#define OUT  0x13
#define IN   0x14
#define NOOP 0X15
#define HERE 0X16


char * instructions[] =
  {
    "HALT ",
    "SET  ",
    "PUSH ",
    "POP  ",
    "EQ   ",
    "GT   ",
    "JMP  ",
    "JT   ",
    "JF   ",
    "ADD  ",
    "MULT ",
    "MOD  ",
    "AND  ",
    "OR   ",
    "NOT  ",
    "RMEM ",
    "WMEM ",
    "CALL ",
    "RETN ",
    "OUT  ",
    "IN   ",
    "NOOP ",
    "HERE "
  };

typedef struct VM_T
{
  uint16_t mem[65536];
  uint16_t stack[65536];
  uint16_t stack_ptr;

  uint16_t reg[8];
  uint16_t pc;
} vm_t;

vm_t vm;

uint8_t reg_index(uint16_t val)
{
  if(val < 32768 || val > 32775)
    {
      printf("❌ Error! Invalid register in reg_index (0x%02x)", val);
      exit(1);
    }
  return val - 32768;
}

void print_regs(void)
{
  char str[256];
  char str2[256];

  if(DEBUG)
    {
      sprintf(str, "Registers:\n");
      sprintf(str2, "  reg_0: 0x%02x\n", vm.reg[0]); strcat(str, str2);
      sprintf(str2, "  reg_1: 0x%02x\n", vm.reg[1]); strcat(str, str2);
      sprintf(str2, "  reg_2: 0x%02x\n", vm.reg[2]); strcat(str, str2);
      sprintf(str2, "  reg_3: 0x%02x\n", vm.reg[3]); strcat(str, str2);
      sprintf(str2, "  reg_4: 0x%02x\n", vm.reg[4]); strcat(str, str2);
      sprintf(str2, "  reg_5: 0x%02x\n", vm.reg[5]); strcat(str, str2);
      sprintf(str2, "  reg_6: 0x%02x\n", vm.reg[6]); strcat(str, str2);
      sprintf(str2, "  reg_7: 0x%02x\n", vm.reg[7]); strcat(str, str2);
      printf("%s", str);
    }
}

uint16_t read_mem(uint16_t idx)
{
  uint16_t ret;
  uint16_t temp = vm.mem[vm.pc + idx];

  if(temp > (0x8000 + 8))
    {
      printf("❌ Error: Invalid number! (read_mem)");
    }

  return ret;
}

uint16_t read_mem_dest(uint16_t idx)
{
  uint16_t ret;
  uint16_t temp = vm.mem[vm.pc + idx];

  //  if(DEBUG)
    {
//      printf("vm.mem[vm.pc + 0]: %d\n", vm.mem[vm.pc + 0]);
//      printf("vm.mem[vm.pc + 1]: %d\n", vm.mem[vm.pc + 1]);
//      printf("vm.mem[vm.pc + 2]: %d\n", vm.mem[vm.pc + 2]);
//      printf("idx: %d\n", idx);
//      printf("temp: %d\n", temp);
    }

    if(temp < 32768)
    {
      printf("* reading memory\n");
      ret = temp;
    }
  else if(temp < (32768 + 8))
    {
      printf("* reading reg %d\n", reg_index(temp));
      ret = vm.reg[reg_index(temp)];
    }
  else
    {
      printf("❌ Error: Invalid number! (read_mem_dest)");
    }

  return ret;
}


void print_pc(void)
{
  if(DEBUG)
    {
      printf("Program Counter: 0x%02x (%02d)\n", vm.pc * 2, vm.pc * 2);
      printf("    Instruction: %s (%02d)\n", instructions[vm.mem[vm.pc]], vm.mem[vm.pc]);
    }
}

void print_stack(void)
{
  char str[256];
  char str2[256];
  int i;

  if(DEBUG)
    {
      sprintf(str, "Stack:\n");
      if(vm.stack_ptr)
        {
          for(i = 0; i < vm.stack_ptr; i++)
            {
              sprintf(str2, "  %d: 0x%02x\n", i, vm.stack[i]);
              strcat(str, str2);
            }
        }
      else
        {
          strcat(str, " STACK EMPTY\n");
        }

      printf("%s", str);
    }
}

void save_reg(uint16_t a, uint16_t b)
{
  vm.reg[reg_index(a)] = b;
}

void save_mem(uint16_t a, uint16_t b)
{
  uint16_t res;

  printf("save_mem()\n");
  printf("a: 0x%02x\n", a);
  printf("b: 0x%02x\n", b);

  if(a < 0x8000)
    {
      printf("Saving Mem");
      vm.mem[a] = b;
    }
  else if( a < (0x8000 + 8))
    {
      printf("Saving Reg");
      save_reg(a, b);
    }
  else
    {
      printf("❌ Error! Invalid data in save_mem (0x%02x)", a);
    }
}

void halt(void)
{
  printf("\nExecuting: HALT\n");
  printf("⬢ System halted!! Exiting.");
  exit(0);
}

void set(void)
{
  uint16_t a;
  uint16_t b;

  printf("\nExecuting: SET\n");
  a = vm.mem[vm.pc + 1];
  b = vm.mem[vm.pc + 2];

  save_mem(a, b);
  vm.pc += 3;
  print_regs();
}

void push(void)
{
  uint16_t a;

  printf("\nExecuting: PUSH\n");
  a = read_mem(1);
  vm.stack[vm.stack_ptr++] = a;
  vm.pc += 2;
}

void pop(void)
{
  uint16_t a;

  printf("\nExecuting: POP\n");
  a = read_mem(1);

  printf("a: 0x%02x\n", a);
  print_stack();
  if(vm.stack_ptr > 0)
    {
      save_reg(a, vm.stack[--vm.stack_ptr]);
    }
  else
    {
      printf("❌ Error: Stack underrun!");
      exit(1);
    }
  vm.pc += 2;
}

void eq(void)
{
  uint16_t a;
  uint16_t b;
  uint16_t c;
  bool     res;

  printf("\nExecuting: EQ\n");
  a = read_mem(1);
  b = read_mem(2);
  c = read_mem(3);

  res = (b == c) ? 1 : 0;
  save_mem(a, res);

  vm.pc += 4;
}

void gt(void)
{
  uint16_t a;
  uint16_t b;
  uint16_t c;
  bool     res;

  printf("\nExecuting: GT\n");
  a = read_mem(1);
  b = read_mem(2);
  c = read_mem(3);

  res = (b > c) ? 1 : 0;
  save_mem(a, res);

  vm.pc += 4;
}

void jmp(void)
{
  uint16_t a;

  printf("\nExecuting: JMP\n");
  a = read_mem(1);
  vm.pc = a;
}

void jt(void)
{
  uint16_t a = read_mem(1);
  uint16_t b = read_mem(2);

  printf("\nExecuting: JT\n");
  printf("\na: %d\n", a);
  printf("\nb: %d\n", b);
  vm.pc = ( ! a) ? (vm.pc + 3) : b;
}

void jf(void)
{
  uint16_t a;
  uint16_t b;

  printf("\nExecuting: JF\n");
  a = read_mem(1);
  b = read_mem(2);
  vm.pc = (! a) ? b : (vm.pc + 3);
}

void add(void)
{
  uint16_t res;
  uint16_t a = read_mem(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);
  print_pc();
  printf("\nExecuting: ADD\n");
  printf("*   *   *   *     a: %d\n", a);
  printf("*   *   *   *     b: %d\n", b);
  printf("*   *   *   *     c: %d\n", c);

  res = ((uint32_t) b + c) % 0x8000;
  printf("*   *   *   *   res: %d\n", res);
  save_mem(a, res);
  a = read_mem(1);
  printf("*   *   *   *     a: %d\n", a);

  vm.pc += 4;
}

void mult(void)
{
  uint16_t res;
  uint16_t a;
  uint16_t b;
  uint16_t c;

  printf("\nExecuting: MULT\n");
  a = read_mem(1);
  b = read_mem(2);
  c = read_mem(3);

  res = (b * c) % 32768;
  save_mem(a, res);

  vm.pc += 4;
}

void mod(void)
{
  uint16_t res;
  uint16_t a;
  uint16_t b;
  uint16_t c;

  printf("\nExecuting: MOD\n");
  a = read_mem(1);
  b = read_mem(2);
  c = read_mem(3);

  res = (b % c) % 32768;
  save_mem(a, res);

  vm.pc += 4;
}

void and(void)
{
  uint16_t res;
  uint16_t a;
  uint16_t b;
  uint16_t c;

  printf("\nExecuting: AND\n");
  a = read_mem(1);
  b = read_mem(2);
  c = read_mem(3);

  res = (b & c) % 32768;
  save_mem(a, res);

  vm.pc += 4;
}

void or(void)
{
  uint16_t res;
  uint16_t a;
  uint16_t b;
  uint16_t c;

  printf("\nExecuting: OR\n");
  a = read_mem(1);
  b = read_mem(2);
  c = read_mem(3);

  res = (b | c) % 32768;
  save_mem(a, res);

  vm.pc += 4;
}

void not(void)
{
  uint16_t res;
  uint16_t a;
  uint16_t b;

  printf("\nExecuting: NOT\n");
  a = read_mem(1);
  b = read_mem(2);

  res = (0b01111111 ^ b) % 32768;
  save_mem(a, res);

  vm.pc += 3;
}

void rmem(void)
{
  uint16_t a;
  uint16_t b;

  printf("\nExecuting: RMEM\n");
  a = read_mem(1);
  b = read_mem(2);

  save_mem(a, b);
  vm.pc += 3;
}

void wmem(void)
{
  uint16_t a;
  uint16_t b;

  printf("\nExecuting: WMEM\n");
  a = read_mem(1);
  b = read_mem(2);

  save_mem(b, a);
  vm.pc += 3;
}

void call(void)
{
  uint16_t a;
  uint16_t pc = vm.pc + 2;

  printf("\nExecuting: CALL\n");
  a = read_mem(1);
  vm.stack[vm.stack_ptr++] = pc;
  vm.pc = a;
}

void retn(void)
{
  uint16_t pc = vm.stack[--vm.stack_ptr];

  printf("\nExecuting: RETN\n");
  vm.pc = pc;
}

void out(void)
{
  uint8_t a = read_mem(1);
  //  printf("\nExecuting: OUT\n");
  //  printf("\nPrinting: %d \n", a);
  printf("\033[31m");
  printf("%c", a);
  printf("\033[0m");
  vm.pc += 2;
}

void in(void)
{
  printf("\nExecuting: IN\n");
  exit(0);
}

void noop(void)
{
  printf("\nExecuting: NOOP\n");
  vm.pc += 1;
}

void act_upon(uint8_t instr)
{
  switch(instr)
    {
    case HALT:
      halt();
      break;

    case SET:
      set();
      break;

    case PUSH:
      push();
      break;

    case POP:
      pop();
      break;

    case EQ:
      eq();
      break;

    case GT:
      printf("here gt\n");
      gt();
      break;

    case JMP:
      jmp();
      break;

    case JT:
      jt();
      break;

    case JF:
      jf();
      break;

    case ADD:
      add();
      break;

    case MULT:
      mult();
      break;

    case MOD:
      mod();
      break;

    case AND:
      and();
      break;

    case OR:
      or();
      break;

    case NOT:
      not();
      break;

    case RMEM:
      rmem();
      break;

    case WMEM:
      wmem();
      break;

    case CALL:
      call();
      break;

    case RETN:
      retn();
      break;

    case OUT:
      out();
      break;

    case IN:
      in();
      printf("\nExecuting: IN\n");
      break;

    case NOOP:
      noop();
      break;

    case HERE:
      printf("********** HERE\n");
      exit(1);
      break;

    default:
      printf("❌ Error! Invalid instruction! (%d)", instr);
      exit(1);
      break;
    }
}

void pc_process(void)
{
  act_upon(vm.mem[vm.pc]);
}

bool read_file(void)
{
  bool ret = false;

  // Pointer to the file to be
  // read from
  FILE * ptr;
  uint16_t c;

  // Stores the bytes to read
  char str[32768];
  int i = 0;
  int j;
  uint16_t val;
  bool lsb;
  int filesize = 59914;
  // If the file exists and has
  // read permission

  ptr = fopen("challenge.bin", "rb");

  if (ptr != NULL)
    {
      // Loop to read required byte
      // of file
      lsb = true;
      for(j = 0; j < filesize; j++)
        {
          c = fgetc(ptr);

          if(lsb)
            {
              lsb = false;
              val = c;
            }
          else
            {
              lsb = true;
              val += (c * 256);
              //               printf("val: 0x%" PRIx16 "\n", val);   // hex

              if(val >= 0x8008)
                {
                  printf("❌ Error: invalid value! (0x%02x)\n", val);
                  exit(1);
                }
              //  if(i > 32) return true;
              vm.mem[i++] = val;
            }
        }

      // Print the bytes as string

      // Close the file
      fclose(ptr);
      ret = true;
    }
  // Input from the user range of
  // bytes inclusive of from and to
  // printf("Read bytes from: ");
  // scanf("%d", &from);
  // printf("Read bytes upto: ");
  // scanf("%d", &to);

  return ret;
}

int main(void)
{
  printf("\nStarting!\n");

  if( ! read_file() )
    {
      printf("❌ Error! read_file failed!\n");
    };

//  printf("Value at address 0: 0x%02x\n", vm.mem[0]);
//  printf("Value at address 1: 0x%02x\n", vm.mem[1]);
//  printf("Value at address 2: 0x%02x\n", vm.mem[2]);

  for(int i = 0; i < 210; i++)
    {
      print_pc();
      pc_process();
    }

  printf("Finish\n");
  exit(0);

  vm.mem[0] = SET;
  vm.mem[1] = 0x8000;
  vm.mem[2] = 0x11;
  pc_process();
  print_stack();

  vm.mem[3] = PUSH;
  vm.mem[4] = 0x22;
  pc_process();
  print_stack();

  vm.mem[5] = POP;
  vm.mem[6] = 0x8000 + 1;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[7] =  EQ;
  vm.mem[8] =  0x8000 + 2;
  vm.mem[9] =  0x22;
  vm.mem[10] = 0x22;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[11] =  EQ;
  vm.mem[12] =  0x8000 + 2;
  vm.mem[13] =  0x22;
  vm.mem[14] =  0x23;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[15] =  GT;
  vm.mem[16] =  0x8000 + 2;
  vm.mem[17] =  0x24;
  vm.mem[18] =  0x22;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[19] =  GT;
  vm.mem[20] =  0x8000 + 2;
  vm.mem[21] =  0x24;
  vm.mem[22] =  0x24;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[23] =  JMP;
  vm.mem[24] =  26;
  vm.mem[25] = HALT; // should be ignored
  pc_process();
  print_regs();
  pc_process();

  vm.mem[26] =  JMP;
  vm.mem[27] =  30;
  vm.mem[28] =  HALT;
  vm.mem[29] =  HALT;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[30] =  JT;
  vm.mem[31] =  1;
  vm.mem[32] =  34;
  vm.mem[33] =  HALT;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[34] =  JT;
  vm.mem[35] =  0;
  vm.mem[36] =  00;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[37] =  JF;
  vm.mem[38] =  0;
  vm.mem[39] =  41;
  vm.mem[40] =  HALT;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[41] =  JF;
  vm.mem[42] =  1;
  vm.mem[43] =  00;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[44] =  ADD;
  vm.mem[45] =  0x8000 + 2;
  vm.mem[46] =  0x22;
  vm.mem[47] =  0x33;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[48] =  MULT;
  vm.mem[49] =  0x8000 + 3;
  vm.mem[50] =  0x10;
  vm.mem[51] =  0x02;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[52] =  MOD;
  vm.mem[53] =  0x8000 + 4;
  vm.mem[54] =  0x05;
  vm.mem[55] =  0x04;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[56] =  AND;
  vm.mem[57] =  0x8000 + 5;
  vm.mem[58] =  0B00000101;
  vm.mem[59] =  0B00001111;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[60] =  OR;
  vm.mem[61] =  0x8000 + 6;
  vm.mem[62] =  0B00000101;
  vm.mem[63] =  0B00001010;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[64] =  NOT;
  vm.mem[65] =  0x8000 + 7;
  vm.mem[66] =  0b00000110; // 0b0111 1001  79
  pc_process();
  print_regs();
  print_pc();

  vm.mem[67] =  RMEM;
  vm.mem[68] =  0x8000 + 0;
  vm.mem[69] =  0x88;
  pc_process();
  print_regs();
  print_pc();


  vm.mem[70] =  WMEM;
  vm.mem[71] =  0x99;
  vm.mem[72] =  0x8000 + 0;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[73] = NOOP;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[74] = NOOP;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[75] = NOOP;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[76] = NOOP;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[77] = CALL;
  vm.mem[78] = 85;
  vm.mem[79] = HALT;
  vm.mem[80] = HALT;
  vm.mem[81] = HALT;
  vm.mem[82] = HALT;
  vm.mem[83] = HALT;
  vm.mem[84] = HALT;
  pc_process();
  print_regs();
  print_pc();

  vm.mem[85] = RETN;
  pc_process();

  vm.mem[79] = CALL;
  vm.mem[80] = 90;

  vm.mem[90] = OUT;
  vm.mem[91] = 'x';
  pc_process();
  print_regs();
  print_pc();


  vm.mem[92] = HERE;
  vm.mem[80] = NOOP;
  vm.mem[81] = NOOP;
  vm.mem[82] = NOOP;
  vm.mem[83] = NOOP;
  vm.mem[84] = NOOP;
  pc_process();
  print_regs();
  print_pc();


  //  pc_process();
  printf("\n Finish \n");
  return 0;
}

//  printf("val2: 0x%02x\n", a);
//  exit(1);

//  vm.pc = 0;
//  vm.mem[0] = HERE;
//  pc_process();
//  print_stack();
