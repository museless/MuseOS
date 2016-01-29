ENTRYPOINT	= 0x2000

ASM		= nasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -minline-all-stringops -Wall
AFLAGS		= -I include/ -I include/ant -c -fno-builtin -minline-all-stringops -Wall
LDFLAGS		= -Ttext $(ENTRYPOINT)

MUSEBOOT	= MuseBoot.bin MuseLoader
MUSEKERNEL	= MuseKernel

OBJS		= kernel/Muse_kernel.o kernel/Muse_main.o kernel/Muse_share_info.o kernel/Muse_syscall.o \
		kernel/syscall/syscall_printx.o kernel/syscall/syscall_block.o kernel/syscall/syscall_sendrecv.o \
		prot/prot_init.o prot/prot_main.o prot/prot_i8259A.o \
		lib/lib_string_app.o lib/lib_port_ctl.o lib/lib_disp.o lib/lib_debug.o lib/lib_string_ctl.o \
		lib/lib_misc.o lib/lib_access_lock.o \
		proc/proc_app.o \
		fs/musefs_main.o fs/musefs_misc.o fs/musefs_file_ctl.o \
		ant/ant_queen.o ant/ant_soldier.o ant/ant_mworker.o ant/ant_sworker.o ant/ant_nursery.o \
		ant/ant_test.o ant/ant_scout.o \
		file/file_memory_ctl.o file/file_memory_protect.o file/file_buffer_ctl.o file/file_tty_ctl.o \
		file/file_console_ctl.o \
		dev/dev_hd_ctl.o dev/dev_keyboard_ctl.o dev/dev_clock_ctl.o dev/dev_fs_init.o \
		test/test.o


.PHONY : everything image clean allclean buildimg 

everything : $(MUSEBOOT) $(MUSEKERNEL)

allclean :
	rm -f $(MUSEBOOT) $(MUSEKERNEL)

clean :
	rm -f $(OBJS) $(MUSEBOOT) $(MUSEKERNEL)

image :	allclean everything buildimg clean

buildimg :
	dd if=MuseBoot.bin of=80m.img bs=1 count=446 conv=notrunc
	dd if=MuseBoot.bin of=80m.img seek=510 skip=510 bs=1 count=2 conv=notrunc
	dd if=MuseLoader of=80m.img seek=13637632 bs=1 count=7000 conv=notrunc
	dd if=MuseKernel of=80m.img seek=14686208 bs=1 count=131072 conv=notrunc
	./node

MuseBoot.bin: boot/MuseBoot.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

MuseLoader: boot/MuseLoader.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(MUSEKERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $(MUSEKERNEL) $^

# kernel dir #
kernel/Muse_kernel.o: kernel/Muse_kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/Muse_main.o: kernel/Muse_main.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/Muse_share_info.o: kernel/Muse_share_info.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/Muse_syscall.o: kernel/Muse_syscall.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<


# kernel/syscall dir #
kernel/syscall/syscall_block.o: kernel/syscall/syscall_block.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/syscall/syscall_printx.o: kernel/syscall/syscall_printx.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/syscall/syscall_sendrecv.o: kernel/syscall/syscall_sendrecv.c
	$(CC) $(CFLAGS) -o $@ $<


# prot dir #
prot/prot_init.o: prot/prot_init.c
	$(CC) $(CFLAGS) -o $@ $<

prot/prot_main.o: prot/prot_main.c
	$(CC) $(CFLAGS) -o $@ $<

prot/prot_i8259A.o: prot/prot_i8259A.c
	$(CC) $(CFLAGS) -o $@ $<


# proc dir #
proc/proc_app.o: proc/proc_app.c
	$(CC) $(CFLAGS) -o $@ $<


# fs dir #
fs/musefs_main.o: fs/musefs_main.c
	$(CC) $(CFLAGS) -o $@ $<

fs/musefs_misc.o: fs/musefs_misc.c
	$(CC) $(CFLAGS) -o $@ $<

fs/musefs_file_ctl.o: fs/musefs_file_ctl.c
	$(CC) $(CFLAGS) -o $@ $<


# lib dir #
lib/lib_string_app.o: lib/lib_string_app.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/lib_string_ctl.o: lib/lib_string_ctl.c
	$(CC) $(CFLAGS) -o $@ $<

lib/lib_port_ctl.o: lib/lib_port_ctl.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/lib_disp.o: lib/lib_disp.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/lib_debug.o: lib/lib_debug.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/lib_misc.o: lib/lib_misc.c
	$(CC) $(CFLAGS) -o $@ $<

lib/lib_access_lock.o: lib/lib_access_lock.c
	$(CC) $(CFLAGS) -o $@ $<


# dev dir #
dev/dev_hd_ctl.o: dev/dev_hd_ctl.c
	$(CC) $(CFLAGS) -o $@ $<

dev/dev_fs_init.o: dev/dev_fs_init.c
	$(CC) $(CFLAGS) -o $@ $<

dev/dev_keyboard_ctl.o: dev/dev_keyboard_ctl.c
	$(CC) $(CFLAGS) -o $@ $<

dev/dev/dev_clock_ctl.o .o: dev/dev/dev_clock_ctl.o .c
	$(CC) $(CFLAGS) -o $@ $<


# file dir #
file/file_tty_ctl.o: file/file_tty_ctl.c
	$(CC) $(CFLAGS) -o $@ $<

file/file_console_ctl.o: file/file_console_ctl.c
	$(CC) $(CFLAGS) -o $@ $<

file/file_buffer_ctl.o: file/file_buffer_ctl.c
	$(CC) $(AFLAGS) -o $@ $<

file/file_memory_ctl.o: file/file_memory_ctl.c
	$(CC) $(AFLAGS) -o $@ $<

file/file_memory_protect.o: file/file_memory_protect.c
	$(CC) $(AFLAGS) -o $@ $<


# ant dir #
ant/ant_queen.o: ant/ant_queen.c
	$(CC) $(AFLAGS) -o $@ $<

ant/ant_soldier.o: ant/ant_soldier.c
	$(CC) $(AFLAGS) -o $@ $<

ant/ant_mworker.o: ant/ant_mworker.c
	$(CC) $(AFLAGS) -o $@ $<

ant/ant_sworker.o: ant/ant_sworker.c
	$(CC) $(AFLAGS) -o $@ $<

ant/ant_nursery.o: ant/ant_nursery.c
	$(CC) $(AFLAGS) -o $@ $<

ant/ant_test.o: ant/ant_test.c
	$(CC) $(AFLAGS) -o $@ $<

ant/ant_scout.o: ant/ant_scout.c
	$(CC) $(AFLAGS) -o $@ $<


# test dir #
test/test.o: test/test.c
	$(CC) $(CFLAGS) -o $@ $<