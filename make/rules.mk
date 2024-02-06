.PRECIOUS: $(BUILD_DIR)/%.o 
.PRECIOUS: $(BUILD_DIR)/samples/%.o 


$(LIBRARY_OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES) $(DEPS)
	@mkdir -p $(@D)
	$(CXXV) $(CFLAGS) $< -o $@ $(INCLUDE_FLAGS) -c

$(BUILD_DIR)/samples/%.o: $(SAMPLES_DIR)/%.cpp $(INCLUDES) $(DEPS)
	@mkdir -p $(@D)
	$(CXXV) $(CFLAGS_INTERNAL) $(CFLAGS) $< -o $@ $(INCLUDE_FLAGS) -c

$(LIBRARY): $(LIBRARY_OBJS) $(DEPS)
	@mkdir -p $(@D)
	$(ARV) cr $(LIBRARY) $(LIBRARY_OBJS)

$(BUILD_DIR)/sample%: $(BUILD_DIR)/samples/sample%.o $(LIBRARY) $(DEPS)
	@mkdir -p $(@D)
	$(CXXLDV) -o $@ $< $(LDFLAGS)

.PHONY: executables
executables: $(SAMPLES)

