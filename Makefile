.PHONY: clean All

All:
	@echo "----------Building project:[ client - Debug ]----------"
	@cd "client" && "$(MAKE)" -f  "client.mk"
clean:
	@echo "----------Cleaning project:[ client - Debug ]----------"
	@cd "client" && "$(MAKE)" -f  "client.mk" clean
