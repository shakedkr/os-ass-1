#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

int index = 0; //index of current place in history(for up and down)
int lCounter =0;

static void consputc(int);

static int panicked = 0;

static struct {
    struct spinlock lock;
    int locking;
} cons;

static void
printint(int xx, int base, int sign) {
    static char digits[] = "0123456789abcdef";
    char buf[16];
    int i;
    uint x;

    if (sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.

void
cprintf(char *fmt, ...) {
    int i, c, locking;
    uint *argp;
    char *s;

    locking = cons.locking;
    if (locking)
        acquire(&cons.lock);

    if (fmt == 0)
        panic("null fmt");

    argp = (uint*) (void*) (&fmt + 1);
    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            consputc(c);
            continue;
        }
        c = fmt[++i] & 0xff;
        if (c == 0)
            break;
        switch (c) {
            case 'd':
            printint(*argp++, 10, 1);
            break;
            case 'x':
            case 'p':
            printint(*argp++, 16, 0);
            break;
            case 's':
            if ((s = (char*) *argp++) == 0)
                s = "(null)";
            for (; *s; s++)
                consputc(*s);
            break;
            case '%':
            consputc('%');
            break;
            default:
                // Print unknown % sequence to draw attention.
            consputc('%');
            consputc(c);
            break;
        }
    }
    if (locking) release(&cons.lock);
}

void
panic(char *s) {
    int i;
    uint pcs[10];

    cli();
    cons.locking = 0;
    cprintf("cpu%d: panic: ", cpu->id);
    cprintf(s);
    cprintf("\n");
    getcallerpcs(&s, pcs);
    for (i = 0; i < 10; i++)
        cprintf(" %p", pcs[i]);
    panicked = 1; // freeze other CPU
    for (;;)
        ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
#define UP 226
#define DOWN 227
#define LEFT 228
#define RIGHT 229
#define MAX_BUF_SIZE 128
#define MAX_HISTORY 16

static ushort *crt = (ushort*) P2V(0xb8000); // CGA memory

#define INPUT_BUF 128

struct {
    struct spinlock lock;
    char buf[INPUT_BUF];
    uint r; // Read index
    uint w; // Write index
    uint e; // Edit index
} input;

static void
cgaputc(int c) {
    int pos;
    // Cursor position: col + 80*row.
    outb(CRTPORT, 14);
    pos = inb(CRTPORT + 1) << 8;
    outb(CRTPORT, 15);
    pos |= inb(CRTPORT + 1);
    char poc [1];
    poc[0] = c;
    if (c == '\n')
        pos += 80 - pos % 80;
    else if (c == BACKSPACE) {
        if (pos > 0 && crt[pos] != 0) {
            memmove(crt + pos - 1, crt + pos, 128);
            --pos;
        } else if (pos > 0) {
            --pos;
            crt[pos] = ' ' | 0x0700;
        }
    } 
    else if(c == UP || c == DOWN)
    {
        
    }
    else if (c == LEFT) {
        if (pos > 0)
            pos--;
    } else if (c == RIGHT) {
        if(pos<25*80)
            pos++;
    }
    else {
        if (pos > 0 && crt[pos] != 0) {
            // change 666 to number of letters
            memmove(crt + pos + 1, crt + pos, 128);
            memmove(crt + pos, poc, 1); //move all crt to right and write to pos
        }
        crt[pos++] = (c & 0xff) | 0x0700; // black on white
    }
    if ((pos / 80) >= 24) { // Scroll up.
        memmove(crt, crt + 80, sizeof (crt[0])*23 * 80);
        pos -= 80;
        memset(crt + pos, 0, sizeof (crt[0])*(24 * 80 - pos));
    }
    outb(CRTPORT, 14);
    outb(CRTPORT + 1, pos >> 8);
    outb(CRTPORT, 15);
    outb(CRTPORT + 1, pos);
}

void
consputc(int c) {
    if (panicked) {
        cli();
        for (;;)
            ;
    }
    if (c == BACKSPACE) {
        uartputc('\b');
        uartputc(' ');
        uartputc('\b');
    } else if(c == RIGHT){
        ;
    } else if(c == LEFT){
        ;
    } else
    uartputc(c);
    cgaputc(c);
}


#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void)) {
    int c;
    // char empty[128];
    int destination_size=0;
    acquire(&input.lock);
    while ((c = getc()) >= 0) {
        switch (c) {
            case C('P'): // Process listing.
            procdump();
            break;
            case C('U'): // Kill line.
            while (input.e != input.w &&
                input.buf[(input.e - 1) % INPUT_BUF] != '\n') {
                input.e--;
            consputc(BACKSPACE);
        }
        break;
            case C('H'): case '\x7f': // Backspace
            if (input.e != input.w) {
                input.e--;
                consputc(BACKSPACE);
            }
            break;
                case 228: // LEFT - tried with \xE4 also
                if (input.e != input.w) {
                // lCounter++;
                    input.e--;
                    lCounter++;
                    cgaputc(LEFT);
                }
                break;
            case 229: // RIGHT - tried with \xE5 also
            if(lCounter>0){
                input.e++;
                lCounter--;
                cgaputc(RIGHT);
            }
            break;
            case 226: // UP
            if (index<= historyCount ) {
                destination_size = strlen (history_buffer[index]);                
                cprintf(history_buffer[index]);
                strncpy(input.buf , history_buffer[index],destination_size-1);
                if (index<historyCount) index++;
                cgaputc(UP);
            }
            break;
            case 227: // DOWN
            if (index >= 0 && index<= historyCount) { 
                destination_size = strlen (history_buffer[index]);
                cprintf(history_buffer[index]); 
                strncpy(input.buf , history_buffer[index],destination_size-1);
                if (index>0) index--;
                cgaputc(DOWN);
            }
            break;
            default:
            if (c != 0 && input.e - input.r < INPUT_BUF) {
                c = (c == '\r') ? '\n' : c;
                input.buf[input.e++ % INPUT_BUF] = c;
                consputc(c);
                if (c == '\n' || c == C('D') || input.e == input.r + INPUT_BUF) {
                    input.w = input.e;
                    wakeup(&input.r);
                }
            }
            break;
        }
    }
    release(&input.lock);
}

int
consoleread(struct inode *ip, char *dst, int n) {
    uint target;
    int c;

    iunlock(ip);
    target = n;
    acquire(&input.lock);
    while (n > 0) {
        while (input.r == input.w) {
            if (proc->killed) {
                release(&input.lock);
                ilock(ip);
                return -1;
            }
            sleep(&input.r, &input.lock);
        }
        c = input.buf[input.r++ % INPUT_BUF];
        if (c == C('D')) { // EOF
            if (n < target) {
                // Save ^D for next time, to make sure
                // caller gets a 0-byte result.
                input.r--;
            }
            break;
        }
        *dst++ = c;
        --n;
        if (c == '\n')
            break;
    }
    release(&input.lock);
    ilock(ip);

    return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n) {
    int i;

    iunlock(ip);
    acquire(&cons.lock);
    for (i = 0; i < n; i++)
        consputc(buf[i] & 0xff);
    release(&cons.lock);
    ilock(ip);

    return n;
}

void
consoleinit(void) {
    initlock(&cons.lock, "console");
    initlock(&input.lock, "input");

    devsw[CONSOLE].write = consolewrite;
    devsw[CONSOLE].read = consoleread;
    cons.locking = 1;

    picenable(IRQ_KBD);
    ioapicenable(IRQ_KBD, 0);
}

/*

            
            case UP:  // UP
            if(index<15){
              index++;
              // if(index==0) 
              //   {
              //     int k = 0;
              //     int l = input.w;
              //     while (l != input.e) {
              //       empty[k] = input.buf[l % INPUT_BUF];
              //       ++l;
              //       ++k;
              //     }
              //   }
                input.e = input.w;
                int k = 0;
                while (history_buffer[index][k] != '\0') {
                  input.buf[input.e % INPUT_BUF] = history_buffer[index][k];
                  ++input.e;
                  ++k;
                }
              }
              cgaputc(UP);
            break;
          case DOWN:  // DOWN
            if(index > -1){
            index--;
            input.e = input.w;
            int i = 0;
            while (history_buffer[index][i] != '\000') {
              input.buf[input.e % INPUT_BUF] = history_buffer[index][i];
              ++input.e;
              ++i;
            }
              cgaputc(DOWN);
            }
            break;



            ------------------------------------------


            else if(c == UP || c == DOWN)
    {
      pos -= (pos % 80);
      pos += 2;
      int i = 0;
      while (history_buffer[index][i] != '\0') {
        crt[pos] = (history_buffer[index][i] & 0xff) | 0x0700;
        ++pos;
        ++i;
    }
    int toClean = pos + 1;
    while(crt[toClean])
    {
        crt[toClean] = ' ' | 0x0700;
        ++toClean;
    }
}
            */