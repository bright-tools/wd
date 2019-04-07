PFX     = 
TGT     = ../src/wd.exe
LIST_FN = ./list
LOG_OUT = out.log
LOG_ERR = err.log
LOG_ERR_T = err.log.tmp
PP_ERR  = sed -e 's/.*wd.exe/wd.exe/;' > $(LOG_ERR)
MORE_PP_ERR = sed -e 's/.*wd.exe/wd.exe/;' >> $(LOG_ERR)
OUT     = 2>$(LOG_ERR_T) 1>$(LOG_OUT) && cat $(LOG_ERR_T) | $(PP_ERR)
MORE_OUT= 2>$(LOG_ERR_T) 1>>$(LOG_OUT) && cat $(LOG_ERR_T) | $(MORE_PP_ERR)
TIME    = $(shell echo $$[$(START)+$(OFF)])
TGT_E   = $(PFX) $(TGT) -f $(LIST_FN) -z $(TIME)
PASS    = win32
START   = 1386181003
OFF     = 0
DIFF_OPT= -c -b --ignore-case
CHECK_E = $(PFX) diff $(DIFF_OPT) $(PASS)/list.$(STEP) list && diff $(DIFF_OPT) $(PASS)/out.log.$(STEP) out.log && diff $(DIFF_OPT) $(PASS)/err.log.$(STEP) err.log

.PHONY: all
all: test22
	@echo -e \\nAll tests run OK

.PHONY: clean
clean:
	@echo Cleaning up
	$(PFX) rm -rf $(LIST_FN)

.PHONY: test1
test1: OFF=0
test1: STEP=1
test1: clean
test1: $(TGT)
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
	@echo Test $(STEP): Dumping list \(Command line option -d\)
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

.PHONY: test10
test10: OFF=45
test10: STEP=10
test10: test9
	@echo Test $(STEP): Testing list output \(vanilla\)
	$(TGT_E) -a 'C:\' see $(OUT)
	$(TGT_E) -a 'C:\windows' windows $(OUT)
	$(TGT_E) -a 'C:\temp' temp $(OUT)
	$(CHECK_E)
	$(TGT_E) -l l $(OUT)

.PHONY: test11
test11: OFF=50
test11: STEP=11
test11: test10
	@echo Test $(STEP): Testing list output \(numbered\)
	$(TGT_E) -l 1l $(OUT)
	$(CHECK_E)

.PHONY: test12
test12: OFF=55
test12: STEP=12
test12: test11
	@echo Test $(STEP): Testing index lookups
	$(TGT_E) -g 0 $(OUT)
	$(TGT_E) -g 1 $(MORE_OUT)
	$(TGT_E) -g 2 $(MORE_OUT)
	$(TGT_E) -g 3 $(MORE_OUT)
	$(CHECK_E)

.PHONY: test13
test13: OFF=60
test13: STEP=13
test13: test12
	@echo Test $(STEP): Adding UNC paths
	$(TGT_E) -a '\\someserver\someshare\somedir' unc1 $(OUT)
	$(TGT_E) -a '\\someserver\someshare' unc2 $(MORE_OUT)
	$(TGT_E) -a //someserver/someothershare unc3 $(MORE_OUT)
	$(CHECK_E)

.PHONY: test14
test14: OFF=65
test14: STEP=14
test14: test13
	@echo Test $(STEP): Retrieving UNC paths
	$(TGT_E) -n unc1 -s c $(OUT)
	$(TGT_E) -n unc1 -s w $(MORE_OUT)
	$(TGT_E) -n unc2 -s c $(MORE_OUT)
	$(TGT_E) -n unc2 -s w $(MORE_OUT)
	$(TGT_E) -n unc3 -s c $(MORE_OUT)
	$(TGT_E) -n unc3 -s w $(MORE_OUT)
	$(CHECK_E)

.PHONY: test15
test15: OFF=60
test15: STEP=15
test15: test14
	@echo Test $(STEP): Removing UNC paths
	$(TGT_E) -r '\\someserver\someshare\somedir' $(OUT)
	$(TGT_E) -r '\\someserver\someshare' $(MORE_OUT)
	$(TGT_E) -r //someserver/someothershare $(MORE_OUT)
	$(CHECK_E)

.PHONY: test16
test16: OFF=65
test16: STEP=16
test16: test15
	@echo Test $(STEP): Command Line Options : -v \(version\)
	$(TGT_E) -v $(OUT)
	$(CHECK_E)

.PHONY: test17
test17: OFF=70
test17: STEP=17
test17: test16
	@echo Test $(STEP): Command Line Options : -h \(help\)
	$(TGT_E) -h $(OUT)
	$(CHECK_E)

.PHONY: test18
test18: OFF=75
test18: STEP=18
test18: test17
	@echo Test $(STEP): Command Line Options : Without -t \(no timestamp update\)
	$(TGT_E) -n see $(OUT)
	$(TGT_E) -g 2 $(MORE_OUT)
	$(CHECK_E)

.PHONY: test19
test19: OFF=80
test19: STEP=19
test19: test18
	@echo Test $(STEP): Command Line Options : With -t \(timestamp update\)
	$(TGT_E) -t -n see $(OUT)
	$(TGT_E) -t -g 2 $(MORE_OUT)
	$(CHECK_E)

.PHONY: test20
test20: OFF=85
test20: STEP=20
test20: test19
	@echo Test $(STEP): Command Line Options : -c \(escape spaces\)
	$(TGT_E) -a '/some directory/here' bn
	$(TGT_E) -a '/another directory/here' et
	$(TGT_E) -a '/usr/bin' ub
	$(TGT_E) -c -l l $(OUT)
	$(CHECK_E)

.PHONY: test21
test21: OFF=90
test21: STEP=21
test21: test20
	@echo Test $(STEP): Command Line Options : -C \(double escape spaces, win form\)
	$(TGT_E) -C -l l -s w $(OUT)
	$(CHECK_E)

.PHONY: test22
test22: OFF=95
test22: STEP=22
test22: test21
	@echo Test $(STEP): Command Line Options : -C \(double escape spaces, cygwin form\)
	$(TGT_E) -C -l l -s c $(OUT)
	$(CHECK_E)
