PFX     = @
TGT     = ../src/wd.exe
LIST_FN = ./list
OUT     = 2>err.log 1>out.log
TIME    = $(shell echo $$[$(START)+$(OFF)])
TGT_E   = $(PFX) $(TGT) -f $(LIST_FN) -z $(TIME)
PASS    = win32
START   = 1386181003
OFF     = 0
CHECK_E = $(PFX) diff list $(PASS)/list.$(STEP) && diff out.log $(PASS)/out.log.$(STEP) && diff err.log $(PASS)/err.log.$(STEP)

.PHONY: all
all: test5

.PHONY: clean
clean:
	@echo Cleaning up
	$(PFX) rm -rf $(LIST_FN)

.PHONY: test1
test1: OFF=0
test1: STEP=1
test1: clean
	@echo Adding a directory
	$(TGT_E) -a 'C:\' see $(OUT)
	$(CHECK_E)

.PHONY: test2
test2: OFF=5
test2: STEP=2
test2: test1
	@echo Dumping list
	$(TGT_E) -d $(OUT)
	$(CHECK_E)

.PHONY: test3
test3: OFF=10
test3: STEP=3
test3: test2
	@echo Adding a directory
	$(TGT_E) -a 'C:\Windows' win $(OUT)
	$(CHECK_E)

.PHONY: test4
test4: OFF=15
test4: STEP=4
test4: test3
	@echo Dumping list
	$(TGT_E) -d $(OUT)
	$(CHECK_E)

.PHONY: test5
test5: OFF=20
test5: STEP=5
test5: test4
	@echo Removing a directory
	$(TGT_E) -r 'C:\Windows' $(OUT)
	$(CHECK_E)

