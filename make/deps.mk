
$(BUILD_DIR)/buildit.dep: $(BUILDIT_DEPS) 
	$(MAKE) -C $(BUILDIT_DIR) DEBUG=$(DEBUG) ENABLE_D2X=$(ENABLE_D2X)
	touch $(BUILD_DIR)/buildit.dep
