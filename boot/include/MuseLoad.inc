MuseLoadSeg	equ	0x9000
MuseLoadOff	equ	0x100
MuseLoadPhyAddr	equ	MuseLoadSeg * 0x10

MuseKerSeg	equ	0x5000
MuseKerOff	equ	0x0
MuseKerPhyAddr	equ	MuseKerSeg * 0x10
MuseKerEntry	equ	0x2000

PageDirBase	equ	0x1000
PageTblBase	equ	0x100000

SUPER_BLK	equ	0x70

ROOT_BASE	equ	0x4EFF

;;-------------------------------------
;; ELEMENT_ADDR have some data about
;; the computer
;;-------------------------------------
ELEMENT_ADDR	equ	0x900

;;-------------------------------------
;;the follow data is in the file sys's
;;super block
;;-------------------------------------
MAGIC_V1		EQU	0x111
SB_MAGIC		EQU	0x0
SB_NR_INODES		EQU	0x4
SB_NR_SECTS		EQU	0x8
SB_NR_IMAP_SECTS	EQU	0xC
SB_NR_SMAP_SECTS	EQU	0x10
SB_N_1ST_SECT		EQU	0x14
SB_NR_INODE_SECTS	EQU	0x18
SB_ROOT_INODE		EQU	0x1C
SB_INODE_SIZE		EQU	0x20
SB_INODE_ISIZE_OFF	EQU	0x24
SB_INODE_START_OFF	EQU	0x28
SB_DIR_ENT_SIZE		EQU	0x2C
SB_DIR_ENT_INODE_OFF	EQU	0x30
SB_DIR_ENT_FNAME_OFF	EQU	0x34