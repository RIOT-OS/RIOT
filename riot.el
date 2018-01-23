;; -*- lexical-binding: t -*-
;; Copyright 2018 John Darrington

;; This file is subject to the terms and conditions of the GNU Lesser General
;; Public License v2.1 or any later version. See the file LICENSE in the top
;; level directory for more details.

;; To use this file put the following in your .emacs file:
;;    (load-file "<path-to-riot-base>/riot.el")
;;
;; You may also want to add the following:
;;    (riot-mode 1)   ;; Starts riot-mode directly
;;

;; This package allows you to develop RIOT code without having to
;; continually leave Emacs and mess around in shell land.  Among other
;; features, it provide commands to build, flash, debug, and test
;; applications.
;;
;; It also provides a minor mode, which 1) sets the environment such
;; that experienced emacsers can use standard Emacs features to build
;; and debug their code; and 2) adds a panel in the menu-bar to assist
;; less experienced users to edit, compile, debug, test and flash
;; their RIOT code.
;;
;; Some convenience functions are included.  For example there is
;; a function to generate and locally browse the RIOT documentation.




;; Default to the value of the environment variable "BOARD" if set.
;; Otherwise default to "native"
(defcustom riot-board
  "native"
  "A string which is the name of the board which RIOT is currently targeting.
It must be a board name which RIOT supports.  It defaults to the value of the
environment variable BOARD if set, or to \"native\" otherwise.

You should not change this value directly.  Use the function `riot-set-board'
or `customize'."
  :type '(string)
  :set (lambda (sym exp) (riot-set-board exp))
  :initialize (lambda (sym exp) (let ((b (getenv "BOARD"))) (set sym (if b b "native"))))
  :group 'riot)

(defconst riot-default-base (file-name-directory load-file-name)
  "The default location of the top level RIOT directory.")

(defcustom riot-project
  ""
  "A string containing the directory where the RIOT project currently in work is
located.


You should not change this value directly.  Use the function `riot-set-project'
or `customize'."
  :type '(string)
  :set (lambda (sym exp) (riot-set-project exp))
  :initialize (lambda (sym exp) (set sym (concat riot-default-base "examples/default")))
  :group 'riot)



(defun filter-out* (pred l x)
  (if l
      (filter-out*
       pred
       (cdr l)
       (if (funcall pred (car l))
	   x
	 (append x (list (car l)))))
    x))

(defun filter-out (pred l)
  "Return a copy of L (a list) with all elements which satisfy PRED removed."
  (filter-out* pred l '()))

(defun riot-base (dir)
  "If DIR is a directory beneath the root of the RIOT source tree then return
the root.  Otherwise return nil."
  (if (member "riot.el" (directory-files dir))
      dir
    (if (equal dir "/") riot-default-base
      (riot-base (expand-file-name (concat dir "/.."))))))


(defun riot-value-of-make-variable (variable)
  "Return the value of the variable called VARIABLE defined in the RIOT makefile
farm."
  ;; Some parameters in which we are interested are set only in the makefiles,
  ;; and are done so in a very ad-hoc manner.  So the only way we can get them
  ;; it is to run make with a custom rule and then print out the result and slurp
  ;; the result.  Yuk!
  (shell-command-to-string
   (mapconcat (function identity)
	      (list "cd" riot-project "&&"
		    (concat "BOARD=" riot-board)
		    "make"
		    (concat "--eval="
			    (shell-quote-argument (concat "riot-el-xall:
	@printf \"$(" variable ")\"")))
		    "riot-el-xall") " ")))

(defun riot-target-arch ()
  "Return the architecture of the current board."
  (riot-value-of-make-variable "TARGET_ARCH"))

(defun riot-application-name ()
  "Return the current application name."
  (riot-value-of-make-variable "APPLICATION"))

(defun riot-bin-dir-base ()
  "Return the name of the directory where the project's binaries reside."
  (riot-value-of-make-variable "BINDIRBASE"))

(defconst riot-debug-server-port "3333")

;; Unfortunately makefiles/cflags.inc.mk totally misuses make, and the
;; end effect is that one cannot pass CFLAGS as a make variable.
;; Instead, it has to be set as an environment variable.  Consequently we
;; have to deal with it specially.

(defun riot-env ()
  "Returns a list of strings of the form VARIABLE=VALUE which should be passed
to riot's environment"
  (if riot-make-variables
      (list
       (let ((env (assoc-string "CFLAGS" riot-make-variables)))
	 (concat (car env) "=" (cdr env))))
    nil))

(defun riot-env-escaped ()
  "Returns a list of strings of the form VARIABLE=VALUE which should be passed
to riot's environment in a bash subshell"
  (if riot-make-variables
      (list
       (let ((env (assoc-string "CFLAGS" riot-make-variables)))
	 (concat (car env) "=" (shell-quote-argument (cdr env)))))
    nil))

(defun riot-make (project board)
  "Return a list of arguments which can be used (for example by start-process)
to build a riot project"
  (append
   `("make" ,(concat "BOARD=" board))
   (if (boundp 'riot-make-variables)
       (mapcar (lambda (z)
		 (concat (car z) "=" (with-output-to-string (princ (cdr z)))))
	       (filter-out
		(lambda (v) (equal (car v) "CFLAGS"))
		riot-make-variables))
     '())
   (list "-C" project)))

;; The inbuilt gud-query-cmdline insists on appending the first file
;; in the current directory.  This is not what we want, so here we
;; provide a simpler definition.
(defun riot-gud-query-cmdline (minor-mode &optional init)
    (read-from-minibuffer
     (format "Run %s (like this): " minor-mode)
     gud-gdb-command-name))


(defun debug-cmdline ()
  "Return a command string suitable for running gdb"
  (combine-and-quote-strings
   (list (if (equal (riot-target-arch) "")
	     "gdb"
	   (concat (riot-target-arch) "-gdb"))
	 "-i=mi"
	 (concat "-iex=file " riot-project "/bin/" riot-board "/"
		 (riot-application-name)
		 ".elf")
	 (concat "-iex=target extended-remote localhost:"
		 riot-debug-server-port))))

(defun stop-debug-server ()
  (let ((proc (get-buffer-process "*riot-debug-server*")))
    (when proc
	(delete-process proc))))

(defun start-make-target (bfr target)
  "Start a process which calls \"make <target>\" in `bfr'.  `bfr' may be nil."
  (let ((process-environment (append process-environment (riot-env))))
    (apply 'start-process target (if bfr bfr (concat "*riot-" target "*"))
	   (append (riot-make riot-project riot-board) (list target)))))

(defun start-debug-server (cmdline)
  (start-make-target nil "debug-server"))

(defun riot-debug ()
  "Debug the running program using GUD.  When `riot-mode' is enabled, you
can use the standard `gdb' command if you prefer."
  (interactive)
  (stop-debug-server)
  (start-debug-server "")
  (gdb (debug-cmdline)))

(defun riot-set-env (project board)
  (setq riot-project project)
  (setq riot-board board)

  (when riot-mode
    ;; The following are customizable variables (not defined by us!!)
    ;; We set them here, and revert them when exiting riot-mode
    (setq compile-command
	  (mapconcat 'identity
	   (append
	    (let ((e (riot-env-escaped)))
	      (if e e nil))
	    (riot-make riot-project riot-board))
	   " "))

    (setq gud-gdb-command-name (debug-cmdline))))

(defgroup riot nil
  "Features to facilitate development for the RIOT operating system."
  :group 'programming
  :link  '(url-link "https://riot-os.org")
  :link  `(file-link ,(concat riot-default-base "README.md")))



;; This is the style preferred by the RIOT developers
(c-add-style "riot" '("linux" (c-basic-offset . 4) (fill-column . 80)))

(define-minor-mode riot-mode
  "Toggle RIOT mode.
     Enables items in the Riot menu for developing RIOT applications."
  :init-value nil
  :lighter " RIOT"
  :global t
  :keymap nil
  :group 'riot
  (if riot-mode
      (progn
	;; Set up a default project/board
	(riot-set-env riot-project riot-board)

	(advice-add 'gud-query-cmdline :override 'riot-gud-query-cmdline)
	(advice-add 'gdb :before 'start-debug-server)

	(when use-riot-indentation
	  (setq  c-default-style
		 `((c-mode . "riot") .
		   ,(if (listp c-default-style)
			c-default-style
		      `((other . c-default-style))))))

	(define-key-after
	  global-map
	  [menu-bar riot]
	  (cons "RIOT" (make-sparse-keymap "riot"))
	  'tools)

	(define-key
	  global-map
	  [menu-bar riot riot-build]
	  '(menu-item "Build" riot-build
		      :help "Build the current project"))

	(define-key-after
	  global-map
	  [menu-bar riot riot-flash]
	  '(menu-item "Flash" riot-flash
		      :help "Upload the binary to a μC board")
	  'riot-build)

	(define-key-after
	  global-map
	  [menu-bar riot riot-term]
	  '(menu-item "Term" riot-term
		      :help "Start a terminal to communicate with a μC board")
	  'riot-flash)

	(define-key-after
	  global-map
	  [menu-bar riot riot-debug]
	  '(menu-item "Debug" riot-debug
		      :help "Debug the program currently running on a μC board")
	  'riot-term)

	(define-key-after
	  global-map
	  [menu-bar riot separator1]
	  '(menu-item "--" t)
	  'riot-debug)

	(define-key-after
	  global-map
	  [menu-bar riot riot-project]
	  '(menu-item "Project..." riot-set-project
		      :help "Set the current Riot project in work")
	  'separator1)

	(define-key-after
	  global-map
	  [menu-bar riot riot-source]
	  '(menu-item "Source" (lambda () (interactive) (riot-project-source riot-project))
		      :help "Dired the source of the current project")
	  'riot-project)

	(define-key-after
	  global-map
	  [menu-bar riot riot-board]
	  `(menu-item "Board..." ,(make-sparse-keymap)
		      :help "Set the type of μC board")
	  'riot-source)

	(mapcar (lambda (brd)
		  (define-key global-map
		    `[menu-bar riot riot-board ,(intern brd)]
		    `(menu-item ,brd (lambda () (interactive)
				       (riot-set-board ,brd)))))
		(reverse (riot-boards)))

	(define-key-after
	  global-map
	  [menu-bar riot separator2]
	  '(menu-item "--" t)
	  'riot-board)

	(define-key-after
	  global-map
	  [menu-bar riot riot-doc]
	  '(menu-item "Docs" riot-doc
		      :help "Browse the RIOT documentation")
	  'separator2)

	(define-key-after
	  global-map
	  [menu-bar riot riot-irc]
	  '(menu-item "IRC" riot-irc
		      :help "Get help on the #riot-os IRC channel")
	  'riot-doc)


	(define-key-after
	  global-map
	  [menu-bar riot riot-cusomize]
	  '(menu-item "Customize" (lambda () (interactive) (customize-group 'riot))
		      :help "Customize riot-mode")
	  'riot-irc))

    (progn
      (advice-remove 'gdb 'start-debug-server)
      (advice-remove 'gud-query-cmdline 'riot-gud-query-cmdline)

      (let ((sv (get 'compile-command 'saved-value)))
        (setq compile-command (if sv sv "make -k")))

      (let ((sv (get 'gud-gdb-command-name 'saved-value)))
        (setq gud-gdb-command-name (if sv sv "gdb -i=mi")))

      (setq c-default-style
            (let ((asc (rassoc "riot" c-default-style)))
              (delete asc c-default-style)))

      (global-unset-key [menu-bar riot]))))



(defcustom riot-make-variables
  nil
  "An alist containing makefile variables to be passed to RIOT's environment.
Some useful values you might want to consider:
'((\"QUIET\"       . \"1\")         ; Show compiler output
  (\"MAKEFLAGS\"   . \"-j 4\"))     ; Build in parallel
"
  :type `(alist :key-type string :value-type string)
  :set (lambda (a b) (progn (set a b) (riot-set-env riot-project riot-board)))
  :options '("CFLAGS" ("QUIET" integer) "MAKEFLAGS")
  :initialize 'custom-initialize-set
  :group 'riot)


(defcustom use-riot-indentation
  nil
  "If not nil, then C indentation style will be set to that preferred
  by the RIOT developers.  Leave this alone if you are not intending
  to contribute code to the RIOT project."
  :type '(boolean)
  :group 'riot)

(defun riot-build ()
  "Build the currently active RIOT project for the current board.
  When `riot-mode' is enabled, you can use the standard `compile'
  command if you prefer."
  (interactive)
  (let ((bfr (get-buffer-create "*riot*")))
    (pop-to-buffer bfr)
    (start-make-target bfr "all")))

(defun riot-flash ()
  "Upload the currently compiled RIOT program into the board's flash memory."
  (interactive)

  ;; If there is a debug server controlling the board then flashing will
  ;; probably fail.  So kill it if there is one.
  (stop-debug-server)
  (let ((bfr (get-buffer-create "*riot*")))
    (pop-to-buffer bfr)
    (start-make-target bfr "flash")))

(defun riot-set-project (project)
  "Set the currently active RIOT \"project\" to PROJECT"
  (interactive "DRiot project: ")
  (riot-set-env (directory-file-name (expand-file-name project))
		riot-board))

(defun riot-project-source (proj)
  "Dired the source directory of the current RIOT project"
  (interactive "DWhich project: ")
  (dired proj))



(require 'term)

(defun lf-crlf-process-filter (process output)
  "Replace LF in output string with CR+LF."
  (term-emulate-terminal process
			 (replace-regexp-in-string "\n" "\r\n" output)))

(defun riot-serial-term-process (port name)
  (serial-supported-or-barf)
  (make-serial-process
   :port port
   :speed 115200
   :bytesize 8
   :parity nil
   :stopbits 1
   :flowcontrol nil
   :coding 'raw-text-unix
   :noquery nil
   :name name
   :filter 'lf-crlf-process-filter
   :sentinel 'term-sentinel))

(defun riot-native-term-process (port name)
  (let ((proc-name
	 (concat "riot-native-process-" port)))
    (signal-process proc-name 'SIGINT)
    (start-process proc-name name
		   (concat (riot-bin-dir-base) "/"
			   riot-board "/"
			   (riot-application-name) ".elf")
		   port)))

(defun riot-term ()
  "Start a terminal to communicate with an attached board"
  (interactive)
  (let* ((port (riot-value-of-make-variable "PORT"))
	 (name (format "*riot-term :%s*" port))
	 (bfr (get-buffer name)))
    (if bfr
	(switch-to-buffer bfr)
      (let* ((process
	      (if (equal riot-board "native")
		  (riot-native-term-process port name)
		(riot-serial-term-process port name)))
	     (buffer (process-buffer process)))
	(with-current-buffer buffer
	  (term-mode)
	  (term-line-mode)
	  (goto-char (point-max))
	  (set-marker (process-mark process) (point)))
	(switch-to-buffer buffer)))))

(defun doc-index () (concat (expand-file-name (riot-base default-directory))
			    "/doc/doxygen/html/index.html"))

(defun show-doc ()
  (browse-url (concat "file://" (doc-index))))

(defun riot-doc ()
  "Browse the RIOT documentation offline."
  (interactive)
  (let ((build-bfr "*riot-make-doc*"))
    (cond
     ;; If the buffer build-bfr exists, then it is probably an existing
     ;; "make doc" in progress.  Switch to it rather than starting another.
     ((member build-bfr
	      (mapcar (lambda (x) (buffer-name x)) (buffer-list)))
      (pop-to-buffer build-bfr))
     ;; If the index already exists, then assume that the docs have already
     ;; been built
     ((file-exists-p (doc-index))
      (show-doc))
     ;; If the documentation has not already been built, then
     ;; (attempt to) build it in a dedicated buffer and run
     ;; show-doc when finished.
     (t (let ((proc
	       (start-make-target build-bfr "doc")))
	  (set-process-sentinel proc
				(lambda (proc what)
				  (when (equal "finished\n" what)
				    (kill-buffer build-bfr)
				    (show-doc))))
	  (switch-to-buffer build-bfr))))))


(require 'erc-track)
(defun riot-irc ()
  "Join the riot-os irc channel."
  (interactive)
  (let* ((bfrs (erc-all-buffer-names))
	 (channel "#riot-os")
	 (server "irc.freenode.net")
	 (sv-bfr (member (concat server ":6667") bfrs))
	 (ch-bfr (member channel bfrs)))
    (if ch-bfr
	(switch-to-buffer (car ch-bfr))
      (progn
	(if sv-bfr
	    (switch-to-buffer (car sv-bfr))
	  (erc :server server))
      (erc-join-channel channel)))))

(defun riot-boards ()
  "Return a list of boards supported by RIOT."
  ;; We do this by examining the directory <riot-base>/boards
  (mapcar (lambda (x) (car x ))
	  (filter-out
	   (function (lambda (file-attr)
		       (let ((dirname (car file-attr))
			     (attr (cdr file-attr)))
			 ;; Ignore files which are not directories.
			 ;; Ignore also ".", ".." and "common"
			 (or
			  (not (car attr))
			  (equal dirname "..")
			  (equal dirname ".")
			  (equal dirname "common")))))
	   (directory-files-and-attributes
	    (concat
	     (riot-base default-directory)
	     "/boards")))))

(defun riot-set-board (board)
  "Set the current RIOT target to BOARD."
  (interactive (list
		(completing-read "Board: " (riot-boards))))
  (riot-set-env riot-project board))

;; Confirm the status of riot-mode
;; This will fix any inconsistent state if this file is loaded after any riot
;; custom variables have been set.
(riot-mode (if riot-mode 1 0))
