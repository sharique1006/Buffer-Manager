# .PHONY : clean_txts clean
all : compile
CC=g++ -O3

sampleobjects = buffer_manager.o file_manager.o
allexecs = linearsearch binarysearch deletion join1 join2 viewfile tester compare

INP = inp1.txt
INP_SAFETY = inp_safety.txt
OUT_CORRECT = out_correct.txt
OUT = out.txt
QUERY = query.txt
INP2 = inp2.txt
DEL_TMP_1 = temp1.txt
DEL_TMP_2 = temp2.txt

LARGENUM = 100000
SMALLNUM = 10000
MAXINARRAY = 350
NUMQUERIES = 400

buffer_manager.o : buffer_manager.cpp
	@ $(CC) -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	@ $(CC) -std=c++11 -c file_manager.cpp

# Algorithms

linearsearch.o : linearsearch.cpp
	@ $(CC) -std=c++11 -c linearsearch.cpp

linearsearch : $(sampleobjects) linearsearch.o constants.h
	@ $(CC) -std=c++11 -o linearsearch $(sampleobjects) linearsearch.o

binarysearch.o : binarysearch.cpp
	@ $(CC) -std=c++11 -c binarysearch.cpp

binarysearch : $(sampleobjects) binarysearch.o constants.h
	@ $(CC) -std=c++11 -o binarysearch $(sampleobjects) binarysearch.o

deletion.o : deletion.cpp
	@ $(CC) -std=c++11 -c deletion.cpp

deletion : $(sampleobjects) deletion.o constants.h
	@ $(CC) -std=c++11 -o deletion $(sampleobjects) deletion.o

join1.o : join1.cpp
	@ $(CC) -std=c++11 -c join1.cpp

join1 : $(sampleobjects) join1.o constants.h
	@ $(CC) -std=c++11 -o join1 $(sampleobjects) join1.o

join2.o : join2.cpp
	@ $(CC) -std=c++11 -c join2.cpp

join2 : $(sampleobjects) join2.o constants.h
	@ $(CC) -std=c++11 -o join2 $(sampleobjects) join2.o

compile : linearsearch binarysearch deletion join1 join2

# Testing functionality

viewfile.o : viewfile.cpp
	@ $(CC) -std=c++11 -c viewfile.cpp

viewfile : $(sampleobjects) viewfile.o constants.h
	@ $(CC) -std=c++11 -o viewfile $(sampleobjects) viewfile.o

tester.o : tester.cpp
	@ $(CC) -std=c++11 -c tester.cpp

tester: $(sampleobjects) tester.o constants.h
	@ $(CC) -std=c++11 -o tester $(sampleobjects) tester.o

compare.o : compare.cpp
	@ $(CC) -std=c++11 -c compare.cpp

compare : $(sampleobjects) compare.o constants.h
	@ $(CC) -std=c++11 -o compare $(sampleobjects) compare.o

# Cleaning functionality

clean_txts :
	@ rm -f $(INP) $(INP_SAFETY) $(OUT_CORRECT) $(OUT) $(QUERY) $(INP2) $(DEL_TMP_1) $(DEL_TMP_2)

clean :
	@ rm -f *.o
	@ rm -f $(allexecs)

# Stress testing

test_linearsearch: tester linearsearch clean_txts
	@ ./tester 0 $(INP) $(QUERY) $(OUT_CORRECT) $(LARGENUM) $(MAXINARRAY) $(NUMQUERIES)
	@ ./linearsearch $(INP) $(QUERY) $(OUT)
	@ echo "Check between these lines"
	@ echo "=================="
	@ diff --binary $(OUT_CORRECT) $(OUT)
	@ echo "=================="

test_binarysearch: tester binarysearch clean_txts
	@ ./tester 1 $(INP) $(QUERY) $(OUT_CORRECT) $(LARGENUM) $(MAXINARRAY) $(NUMQUERIES)
	@ ./binarysearch $(INP) $(QUERY) $(OUT)
	@ echo "Check between these lines"
	@ echo "=================="
	@ diff --binary $(OUT_CORRECT) $(OUT)
	@ echo "=================="

test_deletion: tester deletion viewfile clean_txts
	@ ./tester 2 $(INP) $(QUERY) $(OUT_CORRECT) $(SMALLNUM) $(MAXINARRAY) $(NUMQUERIES)
	@ cp $(INP) $(INP_SAFETY)
	@ ./deletion $(INP) $(QUERY)
	@ ./viewfile $(OUT_CORRECT) > $(DEL_TMP_1)
	@ ./viewfile $(INP) > $(DEL_TMP_2)
	@ echo "Check between these lines"
	@ echo "=================="
	@ diff $(DEL_TMP_1) $(DEL_TMP_2)
	@ echo "=================="

test_join1: tester join1 compare clean_txts
	@ ./tester 3 $(INP) $(INP2) $(OUT_CORRECT) $(SMALLNUM) $(MAXINARRAY) $(NUMQUERIES)
	@ ./join1 $(INP) $(INP2) $(OUT)
	@ ./compare $(OUT_CORRECT) $(OUT)

test_join2: tester join2 compare clean_txts
	@ ./tester 4 $(INP) $(INP2) $(OUT_CORRECT) $(SMALLNUM) $(MAXINARRAY) $(NUMQUERIES)
	@ ./join2 $(INP) $(INP2) $(OUT)
	@ ./compare $(OUT_CORRECT) $(OUT)

test_some:
	@ time make test_linearsearch 
	@ time make test_binarysearch 
	@ time make test_deletion

test_rest:
	@ time make test_join1 
	@ time make test_join2

test_all:
	@ time make test_linearsearch 
	@ time make test_binarysearch 
	@ time make test_deletion 
	@ time make test_join1 
	@ time make test_join2
