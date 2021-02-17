
//atribute initiation method, used to determine the initial positions, velocities, and accelerations of particles
enum attrInitMethod
{
  SET, RANDOM
};
//attributes can either calculate based on lifetime or can be calculated from the next derivative down
enum attrCalcMethod {
  CONSTANT, DERIVATIVE, SCALEDLIFETIME
};
//determines which attribute should be calculated and which should be determined from derivatives
enum derivativeLevel {
  POS, VEL, ACC
};

struct particle {
  float age = 0; //particles will have a life from 1 to 0
  float pos;
  float vel;
  float color;
};

struct particleReference {
  RgbColor color = RgbColor(0, 0, 0);
  float intensity = 0;
};

struct pixelData {
  particleReference refrences[4];
};

class ParticleGenerator : Generator {
    //ATTRIBUTES TO BE CUSTOMIZED
    //decay will be subtracted from particle life every iteration
    float particleDecay = 0.01f;
    //a value starting at 1 will decay at this rate until it reaches 0, where it will be increased by 1
    float timerDecay = 0.01f;

    attrInitMethod posInitMethod = RANDOM;
    attrInitMethod velInitMethod = SET;
    attrInitMethod accInitMethod = SET;
    float posInitValue = 450.0f;
    float velInitValue = 0;
    float accInitValue = 0;

    derivativeLevel calculatedAttribute = ACC;
    attrCalcMethod calcMethod = CONSTANT;
    float attrValue1 = 0.1f;
    float attrValue2 = 0.0f;
    float attrValue3 = 0.0f;

    //DATA USED FOR PARTICLE SIMULATION
    particle particles[255];
    float particleTimer = 1;
    pixelData pixeldata[900];
    
    //METHODS
    particle createParticle() {
      particle p;
      p.age = 1;
      switch (posInitMethod) {
        case SET:
          p.pos = posInitValue;
          break;
        case RANDOM:
          p.pos = randomFloat(450 - posInitValue, 450 + posInitValue);
          break;
      }
      switch (velInitMethod) {
        case SET:
          p.vel = velInitValue;
          break;
        case RANDOM:
          p.vel = randomFloat(-velInitValue, velInitValue);
          break;
      }
      return p;
    }

    void handleCreation(float delta) {
      particleTimer -= timerDecay * delta;
      bool shouldCreateParticle;
      if (particleTimer <= 0) {
        shouldCreateParticle = true;
        particleTimer += 1;
      }
      for (particle p : particles) {
        if (p.age <= 0) {
          if (shouldCreateParticle) {
            p = createParticle();
            shouldCreateParticle = false;
            break;
          }
        }
      }
    }
    void updateParticles(float delta) {

    }
  public:
    void update(float delta) {
      handleCreation(delta);
    }
};
