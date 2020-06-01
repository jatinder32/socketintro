CC=gcc
CFLAG= -g -Wall

SUBDIRS:= server client

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: clean
clean:
	for dir in $(SUBDIRS); do 		\
		$(MAKE) -C $$dir -f Makefile $@; \
	done

