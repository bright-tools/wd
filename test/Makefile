PFX     = @
TGT     = ../src/wd.exe
LIST_FN = ./list
OUT     = 2>err.log.tmp 1>out.log && cat err.log.tmp | sed -e 's/.*wd.exe/wd.exe/;' > err.log
TIME    = $(shell echo $$[$(START)+$(OFF)])
TGT_E   = $(PFX) $(TGT) -f $(LIST_FN) -z $(TIME)
PASS    = win32
START   = 1386181003
OFF     = 0
CHECK_E = $(PFX) diff list $(PASS)/list.$(STEP) && diff out.log $(PASS)/out.log.$(STEP) && diff err.log $(PASS)/err.log.$(STEP)

.PHONY: all
all: test9
	@echo -e \\nAll tests run OK

.PHONY: clean
clean:
	@echo Cleaning up
	$(PFX) rm -rf $(LIST_FN)

.PHONY: test1
test1: OFF=0
test1: STEP=1
test1: clean
	@echo Test $(STEP): Adding a directory
	$(TGT_E) -a 'C:\' see $(OUT)
	$(CHECK_E)

.PHONY: test2
test2: OFF=5
test2: STEP=2
test2: test1
	@echo Test $(STEP): Dumping list
	$(TGT_E) -d $(OUT)
	$(CHECK_E)

.PHONY: test3
test3: OFF=10
test3: STEP=3
test3: test2
	@echo Test $(STEP): Adding a directory
	$(TGT_E) -a 'C:\Windows' win $(OUT)
	$(CHECK_E)

.PHONY: test4
test4: OFF=15
test4: STEP=4
test4: test3
	@echo Test $(STEP): Dumping list
	$(TGT_E) -d $(OUT)
	$(CHECK_E)

.PHONY: test5
test5: OFF=20
test5: STEP=5
test5: test4
	@echo Test $(STEP): Removing a directory
	$(TGT_E) -r 'C:\Windows' $(OUT)
	$(CHECK_E)

.PHONY: test6
test6: OFF=25
test6: STEP=6
test6: test5
	@echo Test $(STEP): Adding a duplicate directory
	$(TGT_E) -a 'C:\' $(OUT)
	$(CHECK_E)

.PHONY: test7
test7: OFF=30
test7: STEP=7
test7: test6
	@echo Test $(STEP): Removing a directory \(list now empty\)
	$(TGT_E) -r 'C:\' $(OUT)
	$(CHECK_E)

.PHONY: test8
test8: OFF=35
test8: STEP=8
test8: test7
	@echo Test $(STEP): Adding a directory with a space in path
	$(TGT_E) -a 'C:\Sill Directory' silly $(OUT)
	$(CHECK_E)

.PHONY: test9
test9: OFF=40
test9: STEP=9
test9: test8
	@echo Test $(STEP): Removing a directory with a space in path
	$(TGT_E) -r 'C:\Sill Directory' $(OUT)
	$(CHECK_E)
