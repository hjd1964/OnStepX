// Sample plugin
#pragma onced

class Sample {
public:
  // the initialization method must be present and named: void init();
  void init();

  void poll();

private:

};

extern Sample sample;
