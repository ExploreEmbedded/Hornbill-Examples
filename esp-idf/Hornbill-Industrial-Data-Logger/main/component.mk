
COMPONENT_EXTRA_CLEAN := certs.h

main.o: certs.h

certs.h: $(COMPONENT_PATH)/certs/root-CA.crt $(COMPONENT_PATH)/certs/certificate.pem.crt $(COMPONENT_PATH)/certs/private.pem.key
	$(Q) rm -f certs.h
	$(Q) echo "const char *rootCA =" >> certs.h
	$(Q) cat $(COMPONENT_PATH)/certs/root-CA.crt | tr -d "\r" | awk '{ print "\""$$0"\\r\\n\""}' >> certs.h
	$(Q) echo ";" >> certs.h
	$(Q) echo "const char *devicePrivateKey =" >> certs.h
	$(Q) cat $(COMPONENT_PATH)/certs/private.pem.key | tr -d "\r" | awk '{ print "\""$$0"\\r\\n\""}' >> certs.h
	$(Q) echo ";" >> certs.h
	$(Q) echo "const char *deviceCert =" >> certs.h
	$(Q) cat $(COMPONENT_PATH)/certs/certificate.pem.crt | tr -d "\r" | awk '{ print "\""$$0"\\r\\n\""}' >> certs.h
	$(Q) echo ";" >> certs.h

