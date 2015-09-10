#lang racket/load
(require racket/gui/base)
(require racket/draw)
(require (planet soegaard/gzip:2:2))


;***************************
; Graphical User Interface
;***************************
; The main window
(define *main-window* (new frame% (width 800) (height 600) (label "NetPaint")))

; The internal image representing the drawing area
(define *drawing-area-width* 800)
(define *drawing-area-height* 600)
(define *drawing-area* (make-bitmap *drawing-area-width* *drawing-area-height* #t))

; A new class for the canvas, inherited from canvas%
(define netpaint-canvas%
  (class canvas%
    (define/override (on-event event) ; Mouse events are sent to the active tool
      (send *active-tool* on-event event))
    (super-new)))

; Drawing context for the canvas
(define (canvas-renderer canvas dc)
  (let-values (((w h) (send canvas get-virtual-size)))
    (send dc draw-bitmap *drawing-area* 0 0)))

; Instantiation of the canvas class, put into the main window
(define *paint-canvas* (new netpaint-canvas% (parent *main-window*) (paint-callback canvas-renderer)))

; A panel into which we put our color panel and tool panel, must be put into main window _after_ the canvas, in order
; to place it on the bottom of the window.
(define *options-panel* (new vertical-panel% (parent *main-window*) (stretchable-height #f)))


;*********************
; Color configuration
;*********************

; Panel onto which we put the color buttons.
(define *colors-panel* (new horizontal-panel% (parent *options-panel*) (alignment '(center center))))

; The currently active color, d'uh. Default color is black, d'uh again.
(define *active-color* (make-object color% "black"))

(define (set-color color)
    (set! *active-color* color))

; Color button maker
(define (make-color-button-new color)
  (define r (send color red))
  (define g (send color green))
  (define b (send color blue))
  (let ((lbl (make-bitmap 32 16)))
    (send lbl set-argb-pixels 0 0 32 16 (apply bytes-append (make-list (* 32 16) (bytes 255 r g b))))
    (new button% (parent *colors-panel*) (label lbl) (callback (lambda (btn event) (begin
                                                                                     (set-color color)
                                                                                     (send btn set-label "1234")))))))
; Make color buttons
(make-color-button-new (make-object color% "black"))
(make-color-button-new (make-object color% "white"))
(make-color-button-new (make-object color% "gray"))
(make-color-button-new (make-object color% "red"))
(make-color-button-new (make-object color% "green"))
(make-color-button-new (make-object color% "blue"))
(make-color-button-new (make-object color% "magenta"))
(make-color-button-new (make-object color% "yellow"))
(make-color-button-new (make-object color% "cyan"))


(define (bgra-to-argb-real! bstr index)
  (cond
    ((< index (bytes-length bstr))
     (let (
           (a (bytes-ref bstr (+ index 3)))
           (r (bytes-ref bstr (+ index 2)))
           (g (bytes-ref bstr (+ index 1)))
           (b (bytes-ref bstr index)))
       (bytes-set! bstr index a)
       (bytes-set! bstr (+ index 1) r)
       (bytes-set! bstr (+ index 2) g)
       (bytes-set! bstr (+ index 3) b))
     (bgra-to-argb-real! bstr (+ index 4)))))
(define (bgra-to-argb! bstr) (bgra-to-argb-real! bstr 0))
(define (argb-to-bgra! bstr) (bgra-to-argb-real! bstr 0))

;*****************
;Tools
;*****************

; Line class
(define tool-line%
  (class object%
    (define startx 0)
    (define starty 0)
    (define isPainting #f)
    (define endx 0)
    (define endy 0)
    (define/public (on-event event) ; Mouse event
      (begin
        (when (equal? (send event get-event-type) 'left-down)
          (set! startx (send event get-x))
          (set! starty (send event get-y))
          (set! isPainting #t))
        (when (equal? (send event get-event-type) 'left-up)
          (set! isPainting #f)
          (set! endx (send event get-x))
          (set! endy (send event get-y))
          ; Generate a line and send it to the image
          (define dc (new bitmap-dc% [bitmap *drawing-area*]))
          (define pen (new pen% [color *active-color*]))
          (send dc set-pen pen)
          (send dc draw-line startx starty endx endy)
          (send *paint-canvas* refresh-now))))
      (super-new)))

; Line with networking class
(define tool-line-networking%
  (class object%
    (define startx '())
    (define endx '())
    (define starty '())
    (define endy '())
    (define isPainting #f)
    (define temp-image '())
    (define uncompressed-pixel-array '())
    (define compressed-pixel-array '())
    (define message-type '())
    (define message-size '())
    (define compressed-size "")
    (define uncompressed-size "")
    (define protocol "")
    (define canvas-width "")
    (define canvas-height "")
    (define canvas-size "")
    (define new-canvas '())
    (define new-dc '())
    (define new-pen '())
    (define file '())
    (define compressed-canvas '())
    (define compressed-canvas-port '())
    (define uncompressed-canvas '())
    (define uncompressed-canvas-port '())
    (define/public (on-event event) ; Mouse event
      (begin
        (when (equal? (send event get-event-type) 'left-down)
          (set! startx (send event get-x))
          (set! starty (send event get-y))
          (set! isPainting #t))
        (when (equal? (send event get-event-type) 'left-up)
          (set! endx (send event get-x))
          (set! endy (send event get-y))
          
          ; Generate a line on an image
          ;(set! temp-image (make-bitmap *drawing-area-width* *drawing-area-height* #t))
          (define dc (new bitmap-dc% [bitmap *drawing-area*]))
          (define pen (new pen% [color *active-color*]))
          (send dc set-pen pen)
          (send dc draw-line startx starty endx endy)
          
          ; Get all pixels into an array
          (set! uncompressed-canvas (make-bytes (* *drawing-area-width* *drawing-area-height* 4)))
          (send *drawing-area* get-argb-pixels 0 0 *drawing-area-width* *drawing-area-height* uncompressed-canvas)
          
          ; Set alpha channel to 0xFF on all pixels that are not white
          
          ; Compress image?
          (argb-to-bgra! uncompressed-canvas)
          (set! compressed-canvas (compress-bytes uncompressed-canvas))
          (set! compressed-size (bytes-length compressed-canvas))
          
          ; Send header to server
          (write-bytes (integer->integer-bytes (+ compressed-size 20) 4 #f #t) *outport*)
          (write-bytes (integer->integer-bytes 2 4 #f #t) *outport*)
          (write-bytes (integer->integer-bytes *drawing-area-width* 4 #f #t) *outport*)
          (write-bytes (integer->integer-bytes *drawing-area-height* 4 #f #t) *outport*)
          (write-bytes (integer->integer-bytes (* *drawing-area-width* *drawing-area-height* 4) 4 #f #t) *outport*)
          (write-bytes compressed-canvas *outport*)
          (flush-output *outport*))))
          
          ; Send image to server
          
          
    (super-new)))

; Freehand class
; UNIMPLEMENTED!
(define tool-freehand%
  (class object%
    (define points '())
    (define uncompressed-canvas '())
    (define compressed-canvas '())
    (define/public (on-event event) ; Mouse event
      (cond
        ((equal? (send event get-event-type) 'left-down)
         (set! points '()))
        ((equal? (send event get-event-type) 'motion)
         (let ((x (send event get-x)) (y (send event get-y)))
           (set! points (append points (list (list x y))))))
        ((equal? (send event get-event-type) 'left-up)
         
         
         (define dc (new bitmap-dc% [bitmap *drawing-area*]))
         (define pen (new pen% [color *active-color*]))
         (send dc set-pen pen)
         (let ((last-x (car (car points))) (last-y (car (cdr (car points)))))
           (for-each (lambda (point)
                       (send dc draw-line last-x last-y (car point) (car (cdr point)))
                       (set! last-x (car point))
                       (set! last-y (car (cdr point))))
                     (cdr points)))
         
         ; Get all pixels into an array
         (set! uncompressed-canvas (make-bytes (* *drawing-area-width* *drawing-area-height* 4)))
         (send *drawing-area* get-argb-pixels 0 0 *drawing-area-width* *drawing-area-height* uncompressed-canvas)
         
         ; Set alpha channel to 0xFF on all pixels that are not white
         
         ; Compress image?
         (argb-to-bgra! uncompressed-canvas)
         (set! compressed-canvas (compress-bytes uncompressed-canvas))
         
         ; Send header to server
         (write-bytes (integer->integer-bytes (+ (bytes-length compressed-canvas) 20) 4 #f #t) *outport*)
         (write-bytes (integer->integer-bytes 2 4 #f #t) *outport*)
         (write-bytes (integer->integer-bytes *drawing-area-width* 4 #f #t) *outport*)
         (write-bytes (integer->integer-bytes *drawing-area-height* 4 #f #t) *outport*)
         (write-bytes (integer->integer-bytes (* *drawing-area-width* *drawing-area-height* 4) 4 #f #t) *outport*)
         (write-bytes compressed-canvas *outport*)
         (flush-output *outport*))))
    
    
    (super-new)))

; Instantiation of the tool classes
(define *tool-line-networking* (new tool-line-networking%))
(define *tool-line* (new tool-line%))
(define *tool-freehand* (new tool-freehand%))


;***********************
; Tool buttons
;***********************

; A panel onto which we put the tool buttons
(define *tools-panel* (new horizontal-panel% (parent *options-panel*) (alignment '(center center))))

; Default tool
(define *active-tool* '())

; We want to have the ability to set a tool without a button being pressed, therefore we have a separate function for this
(define (set-tool tool) (begin
                          ; More to come...
                          (set! *active-tool* tool)))

; Default tool is the line tool
(set-tool *tool-line*)

; Tool button maker
(define (make-tool-button text tool)
  (new button% (parent *tools-panel*)
       (label text) (callback (lambda (btn event)
                   (begin (display "Active tool is now: ")
                          (display text)
                          (newline)
                          (set! *active-tool* tool))))))

; Make the buttons
(make-tool-button "line" *tool-line*)
(make-tool-button "freehand" *tool-freehand*)
(make-tool-button "line networking" *tool-line-networking*)

;*********************
; Show the window
;*********************

(send *main-window* show #t)


;*********************
; Server listener
;*********************

; Some variablels
(define *server-host* "172.20.10.2")
; Will be changed to an actual port once we're connected
(define *outport* 1234)
(define *inport* '())
(define *listen-thread* '())

(define (connect)
  (set!-values (*inport* *outport*) (tcp-connect *server-host* *outport*))
  (set! *listen-thread* (thread listener))
  (display "Connecting to server...")
  (newline))

(define (listener)
  (when (null? *inport*)
    (error "Not connected!"))
  (let ((message "")
        (message-size "")
        (compressed-size "")
        (uncompressed-size "")
        (protocol "")
        (canvas-width "")
        (canvas-height "")
        (canvas-size "")
        (new-canvas '())
        (new-dc '())
        (new-pen '())
        (file '())
        (compressed-canvas '())
        (compressed-canvas-port '()) ; Testing... shit still ain't workin'
        (uncompressed-canvas '())
        (uncompressed-canvas-port '()))
    (define (loop)
      ; The two last boolean arguments to integer-bytes->integer are: signed? and big-endian?
      (set! message-size (integer-bytes->integer (read-bytes 4 *inport*) #f #t))
      (if (eof-object? message)  ; EOF indicates that the server has disconnected
          (begin
            (tcp-abandon-port *outport*)
            (tcp-abandon-port *inport*)
            (display "Disconnected!")
            (newline)
            (kill-thread (current-thread)))
          (begin
            ; The first four-byte integer is the total size of the message
            
            ; Actually, only one protocol type (type 3) has been implemented...
            (set! protocol (integer-bytes->integer (read-bytes 4 *inport*) #f #t))
            
            ; Then comes the canvas size
            (set! canvas-width (integer-bytes->integer (read-bytes 4 *inport*) #f #t))
            (set! canvas-height (integer-bytes->integer (read-bytes 4 *inport*) #f #t))
            
            ; Calculate the canvas' size in byte
            (set! canvas-size (* canvas-width canvas-height 4))
            (read-bytes 4 *inport*)
            (set! uncompressed-canvas (uncompress-bytes (read-bytes (- message-size 20) *inport*) canvas-size))
            ; In fact, server sends BGRA.
            (bgra-to-argb! uncompressed-canvas)
            (send (new bitmap-dc% [bitmap *drawing-area*]) set-argb-pixels 0 0 canvas-width canvas-height
                  uncompressed-canvas)
            
            ; Send it to the painting area
            (send *paint-canvas* refresh-now)
            (display "Canvas refreshed!") (newline)
                       
            ; Wait for another message from the server
            (loop))))
    (loop)))

(define (disconnect)
  (kill-thread listener-thread)
  (tcp-abandon-port *outport*)
  (tcp-abandon-port *inport*)
  (display "Disconnected!"))

; Connect to server!
(connect)
