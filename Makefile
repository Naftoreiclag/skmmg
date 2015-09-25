.PHONY: clean All

All:
	@echo "----------Building project:[ client - Debug ]----------"
	@cd "client" && "$(MAKE)" -f  "client.mk" && "$(MAKE)" -f  "client.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ client - Debug ]----------"
	@cd "client" && "$(MAKE)" -f  "client.mk" clean
