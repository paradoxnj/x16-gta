;************************************************************************************
; File: gtx16.asm
; Author:  Anthony Rufrano (paradoxnj)
; Description:  Grand Theft Auto Clone for Commander X16
;************************************************************************************

;************************************************************************************
; Start on $0801
;************************************************************************************
*=$0801

; 10 SYS 2062
!byte $0B,$08,$0A,$00,$9E,$20,$32,$30,$36,$32,$00,$00,$00


;************************************************************************************
; Includes
;************************************************************************************
!src "x16.inc"
!src "ym2151.asm"

jmp start

;************************************************************************************
; Function:     x16_vera_init
; Arguments:    None
; Description:  Initialize VERA graphics modes
;************************************************************************************
x16_vera_init:
    ;*******************************************
    ; Tell VERA which data port to use
    ;*******************************************
    stz VERA_ctrl

    +VERA_SET_ADDR VRAM_palette,1
    ldx #2
    ldy #0
    lda #<pal
    sta ZP_PTR_2
    lda #>pal
    sta ZP_PTR_3

ploop:
    lda (ZP_PTR_2),y
    sta VERA_data0
    iny
    bne ploop
    inc ZP_PTR_3
    dex
    bne ploop

    +VERA_SET_ADDR $00000,1

    ldx #$50
    ldy #0
    lda #<cars
    sta ZP_PTR_2
    lda #>cars
    sta ZP_PTR_3
carloop:
    lda (ZP_PTR_2),y
    sta VERA_data0
    iny
    cpy $50
    bne carloop
    inc ZP_PTR_3
    dex
    bne carloop
    
    +VERA_SET_ADDR VRAM_layer0, 0
    lda #$00
    ;ora VERA_data0
    sta VERA_data0
    
    ;*******************************************
    ; Use Layer 1 for graphics
    ;*******************************************
    +VERA_SET_ADDR VRAM_layer1,1
    lda #7 << 5 | 1                             ; 4bpp Bitmap Mode
    sta VERA_data0

    stz VERA_data0                              ; TileW = 320
    stz VERA_data0                              ; Map Base L not used
    stz VERA_data0                              ; Map Base H not used
    stz VERA_data0                              ; Tile Base L
    stz VERA_data0                              ; Tile Base H
    stz VERA_data0                              ; HSCROLL Lo
    stz VERA_data0                              ; BM PAL Offset
    stz VERA_data0                              ; VSCROLL Lo
    stz VERA_data0                              ; VSCROLL Hi

    ;*******************************************
    ; Setup the display composer to output VGA
    ;*******************************************
    +VERA_SET_ADDR VRAM_composer, 1
    lda #1
    sta VERA_data0

    
    ;*******************************************
    ; Setup 320x200 resolution
    ;*******************************************
    lda #64
    sta VERA_data0
    sta VERA_data0

    rts

start:
    jsr x16_vera_init

    jmp *
    rts

cars: !bin "cars.x16"

pal: !bin "cars.pal"
