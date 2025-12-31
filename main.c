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

#define IN_VM  true
#define IN_EXT false

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

uint8_t in_state = IN_VM;
uint8_t instance = 0;

uint16_t read_mem(uint16_t idx);
uint16_t read_mem_indirect(uint16_t idx);
void     print_pc(void);
void     print_stack(void);
void     write_reg(uint16_t a, uint16_t b);
uint16_t read_reg(uint16_t a);
void     write_mem_direct(uint16_t a, uint16_t b);
void     write_mem_indirect(uint16_t a, uint16_t b);
void     halt(void);
void     set(void);
void     push(void);
void     pop(void);
void     eq(void);
void     gt(void);
void     jmp(void);
void     jt(void);
void     jf(void);
void     add(void);
void     mult(void);
void     mod(void);
void     and(void);
void     or(void);
void     not(void);
void     rmem(void);
void     wmem(void);
void     call(void);
void     retn(void);
void     out(void);
void     in(void);
void     noop(void);
void     act_upon(uint8_t instr);
void     pc_process(void);
bool     read_file(void);
uint8_t  reg_index(uint16_t val);
void     print_regs(void);

uint16_t read_addr(uint16_t idx)
{
  uint16_t ret;
  uint16_t addr = vm.mem[vm.pc + idx];

  if(addr >= 0x8000 && addr < 0x8008)
    {
      ret = addr;
    }
  else
    {
      printf("❌ Error: Invalid address! (%d) (read_address)", addr);
      exit(1);
    }

  return ret;
}

uint16_t read_mem(uint16_t idx)
{
  uint16_t ret;
  uint16_t temp = vm.mem[vm.pc + idx];

  if(temp < 0x8000)
    {
      ret = temp;
    }
  else if(temp < 0x8008)
    {
      ret = read_reg(temp);
    }
  else
    {
      printf("❌ Error: Invalid number! (read_mem)");
      exit(1);
    }

  return ret;
}

uint16_t read_mem_indirect(uint16_t idx)
{
  uint16_t ret;
  uint16_t addr = vm.mem[vm.pc + idx];

  if(addr < 0x8000)
    {
      ret = vm.mem[addr];
    }
  else if(addr < 0x8008)
    {
      ret = vm.reg[reg_index(addr)];
    }
  else if(addr > (0x8007))
    {
      printf("❌ Error: Invalid number! (read_mem_indirect)");
      exit(1);
    }

  return ret;
}

void print_pc(void)
{
  if(DEBUG)
    {
      printf("Program Counter: 0x%02x (%02d)\n", vm.pc, vm.pc);
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

uint16_t read_reg(uint16_t a)
{
  return vm.reg[reg_index(a)];
}

void write_reg(uint16_t a, uint16_t b)
{
  if(a >= 0x8000 && a < 0x8008)
    {
      vm.reg[reg_index(a)] = b;
    }
  else
    {
      printf("❌ Error: Invalid addr (%d) (write_reg)", a);
      exit(1);
    }
}

void write_mem(uint16_t a, uint16_t b)
{
  uint16_t res;

  if(DEBUG)
    {
      printf("write_mem(): ");
      printf("a: 0x%02x, ", a);
      printf("b: 0x%02x\n", b);
    }
  if(a < 0x8000)
    {
      vm.mem[a] = b;
    }
  else
    {
      printf("❌ Error! Invalid data in write_mem_direct (0x%02x)", a);
      exit(1);
    }
}

/* stop execution and terminate the program */
void halt(void)
{
  exit(0);
}

/* set register <a> to the value of <b> */
void set(void)
{
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);

  if(DEBUG)
    {
      printf("\nExecuting: SET, ");
      printf("a: 0x%02x, ", a);
      printf("b: 0x%02x\n", b);
    }

  write_reg(a, b);
  vm.pc += 3;
}

/* push <a> onto the stack */
void push(void)
{
  uint16_t a = read_mem(1);

if(DEBUG)
  {
    printf("\nExecuting: PUSH, ");
    printf("a: 0x%02x\n", a);
  }

  vm.stack[vm.stack_ptr] = a;
  vm.stack_ptr++;
  vm.pc += 2;
}

/* remove the top element from the stack and write it into <a>; empty stack = error */
void pop(void)
{
  uint16_t a = read_addr(1);

  if(DEBUG)
    {
      printf("\nExecuting: POP, ");
      printf("a: 0x%02x\n", a);
    }

  if(vm.stack_ptr > 0)
    {
      write_reg(a, vm.stack[--vm.stack_ptr]);
    }
  else
    {
      printf("❌ Error: Stack underrun!");
      exit(1);
    }
  vm.pc += 2;
}

/* set <a> to 1 if <b> is equal to <c>; set it to 0 otherwise */
void eq(void)
{
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);
  bool     res;

  if(DEBUG)
    {
      printf("\nExecuting: EQ\n");
      printf("a: 0x%02x\n", a);
      printf("b: 0x%02x\n", b);
      printf("c: 0x%02x\n", c);
    }

  if(b == c)
    {
      res = 1;
    }
  else
    {
      res = 0;
    }

  write_reg(a, res);
  vm.pc += 4;
}

/* set <a> to 1 if <b> is greater than <c>; set it to 0 otherwise */
void gt(void)
{
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);
  bool     res;

  if(DEBUG) printf("\nExecuting: GT\n");

    if(b > c)
    {
      res = 1;
    }
  else
    {
      res = 0;
    }

  write_reg(a, res);

  vm.pc += 4;
}

/* jump to <a> */
void jmp(void)
{
  uint16_t a = read_mem(1);

if(DEBUG)
  {
    printf("\nExecuting: JMP\n");
    printf("Jumping to: 0x%02x\n", a);
  }
  vm.pc = a;
}

/* if <a> is nonzero, jump to <b> */
void jt(void)
{
  uint16_t a = read_mem(1);
  uint16_t b = read_mem(2);

  if(DEBUG)
    {
      printf("\nExecuting: JT: ");
      printf("a: %d, ", a);
      printf("b: %d\n", b);
    }

  if(a)
    {
      vm.pc = b;
    }
  else
    {
      vm.pc += 3;
    }
}

/* if <a> is zero, jump to <b> */
void jf(void)
{
  if(DEBUG) printf("\nExecuting: JF: ");

  uint16_t a = read_mem(1);
  uint16_t b = read_mem(2);

  if( ! a)
    {
      vm.pc = b;
    }
  else
    {
      vm.pc += 3;
    }
}

/* assign into <a> the sum of <b> and <c> (modulo 32768) */
void add(void)
{
  uint16_t res;
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);

  res = ((uint32_t) b + c) % 0x8000;

  if(DEBUG)
    {
      printf("\nExecuting: ADD\n");
      printf("*   *   *   *     a: %d\n", a);
      printf("*   *   *   *     b: %d\n", b);
      printf("*   *   *   *     c: %d\n", c);
      printf("*   *   *   *   res: %d\n", res);
    }
  write_reg(a, res);
  vm.pc += 4;
}

/* store into <a> the product of <b> and <c> (modulo 32768) */
void mult(void)
{
  uint16_t res;
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);

  if(DEBUG) printf("\nExecuting: MULT\n");
  res = ((uint32_t) b * c) % 0x8000;
  write_reg(a, res);

  vm.pc += 4;
}

/* store into <a> the remainder of <b> divided by <c> */
void mod(void)
{
  uint16_t res;
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);

  if(DEBUG) printf("\nExecuting: MOD\n");

  res = ((uint32_t) b % c) % 0x8000;
  write_reg(a, res);

  vm.pc += 4;
}

/* stores into <a> the bitwise and of <b> and <c> */
void and(void)
{
  uint16_t res;
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);

  if(DEBUG) printf("\nExecuting: AND\n");

  res = (b & c) % 0x8000;
  write_reg(a, res);

  vm.pc += 4;
}

/* stores into <a> the bitwise or of <b> and <c> */
void or(void)
{
  uint16_t res;
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);
  uint16_t c = read_mem(3);

  if(DEBUG) printf("\nExecuting: OR\n");

  res = (b | c) % 0x8000;
  write_reg(a, res);

  vm.pc += 4;
}

/* stores 15-bit bitwise inverse of <b> in <a> */
void not(void)
{
  uint16_t res;
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);

  if(DEBUG) printf("\nExecuting: NOT\n");

  res = (~b) & 0x7FFF;
  write_reg(a, res);

  vm.pc += 3;
}

/* read memory at address <b> and write it to <a> */
void rmem(void)
{
  uint16_t a = read_addr(1);
  uint16_t b = read_mem(2);

  if(DEBUG) printf("\nExecuting: RMEM\n");

  write_reg(a, vm.mem[b]);
  vm.pc += 3;
}

/* write the value from <b> into memory at address <a> */
void wmem(void)
{
  uint16_t a = read_mem(1);
  uint16_t b = read_mem(2);

  if(DEBUG) printf("\nExecuting: WMEM\n");
  a = read_mem(1);
  b = read_mem(2);

  vm.mem[a] = b;

  vm.pc += 3;
}

/* write the address of the next instruction to the stack and jump to <a> */
void call(void)
{
  uint16_t a;
  uint16_t pc = vm.pc + 2;

  if(DEBUG) printf("\nExecuting: CALL\n");
  a = read_mem(1);
  if(a == 6049)
    {
      vm.reg[0] = 6;
      vm.reg[7] = 25734;
      vm.pc += 2;
    }
  else
    {
      vm.stack[vm.stack_ptr++] = pc;
      vm.pc = a;
    }
}

/* remove the top element from the stack and jump to it; empty stack = halt */
void retn(void)
{
  uint16_t pc = vm.stack[--vm.stack_ptr];

  if(DEBUG) printf("\nExecuting: RETN\n");
  vm.pc = pc;
}

/* write the character represented by ascii code <a> to the terminal */
void out(void)
{
  char a = read_mem(1);

  printf("%c", a);
  vm.pc += 2;
}

/* read a character from the terminal and write its ascii code to <a> */
void in(void)
{
  uint16_t a;
  char c;

  scanf("%c", &c);

  switch(in_state)
    {
    case IN_VM:
      if(c == '~')
        {
          in_state = IN_EXT;
        }
      else
        {
          a = read_addr(1);
          write_reg(a, c);
          vm.pc += 2;
        }
      break;

    case IN_EXT:
      if(c == '\n')
        {
          in_state = IN_VM;
          vm.reg[7] = 25734;
          printf("What do you do?\n");
        }
      break;

    default:
      printf("❌ Error invalid in_state!");
      exit(1);
      break;
    }
}

/* no operation */
void noop(void)
{
  if(DEBUG) printf("\nExecuting: NOOP\n");
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

  FILE * ptr;
  uint16_t c;
  char str[0x8000];
  int i = 0;
  int j;
  uint16_t val;
  bool lsb;
  int filesize = 59914;

  ptr = fopen("challenge.bin", "rb");

  if (ptr != NULL)
    {
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

              if(val >= 0x8008)
                {
                  printf("❌ Error: invalid value! (0x%02x)\n", val);
                  exit(1);
                }
              vm.mem[i++] = val;
            }
        }

      fclose(ptr);
      ret = true;
    }

  return ret;
}

uint8_t reg_index(uint16_t val)
{
  if(val < 0x8000 || val > 0x8007)
    {
      printf("❌ Error! Invalid register in reg_index (0x%02x)", val);
      exit(1);
    }
  return val - 0x8000;
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

int main(void)
{
  uint16_t addr = 0;

  vm.pc = 0;
  if( ! read_file() )
    {
      printf("❌ Error! read_file failed!\n");
      exit(1);
    }

  for(;;)
    {
      print_pc();
      pc_process();
    }

  printf("\n Finish \n");
  return 0;
}
