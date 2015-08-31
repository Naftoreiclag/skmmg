.PHONY: clean All

All:
	@echo "----------Building project:[ client - Release ]----------"
	@cd "client" && "$(MAKE)" -f  "client.mk"
clean:
	@echo "----------Cleaning project:[ client - Release ]----------"
	@cd "client" && "$(MAKE)" -f  "client.mk" clean
