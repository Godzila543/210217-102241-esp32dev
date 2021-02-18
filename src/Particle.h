#define NUM_PARTICLES 100
//atribute initiation method, used to determine the initial positions, velocities, and accelerations of particles
enum attrInitMethod
{
  SET,
  RANDOM
};
//attributes can either calculate based on lifetime or can be calculated from the next derivative down
enum attrCalcMethod
{
  CONSTANT,
  SCALEDLIFETIME
};
//determines which attribute should be calculated and which should be determined from derivatives
enum derivativeLevel
{
  POS,
  VEL,
  ACC
};

//A data structure for holding information relevant to particles
struct particle
{
  float life = 0; //particles will have a life from 1 to 0
  float pos;
  float vel;
  float color;
};

//A reference to a specific particle with information used to calculate the color
struct particleReference
{
  RgbColor color = RgbColor(0, 0, 0);
  float influence = 0.01;
};

//Holds all the refrences for a given pixel required to calculate the final color
struct pixelData
{
  particleReference refrences[4];
};

//generator for determining pixel colors based on a simulated particle system
class ParticleGenerator : Generator
{
  //ATTRIBUTES TO BE CUSTOMIZED

  float particleDecay = 0.01f; //decay will be subtracted from particle life[1, 0] every iteration
  float timerDecay = 0.1f;     //a timer in the range of [1, 0] will be subtracted by this

  attrInitMethod posInitMethod = RANDOM; //Initiaion method for position attribute of particles
  attrInitMethod velInitMethod = RANDOM; //Initiaion method for velocity attribute of particles
  float posInitValue = 450.0f;           //Range/value of position
  float velInitValue = 1.0;              //Range/value of velocity

  derivativeLevel calculatedAttribute = ACC; //The attribute of the particle to calculate, which will be integrated to find pos/vel
  attrCalcMethod calcMethod = CONSTANT;      //method to calculate value of calculatedAttribute
  float attrValue1 = 0.0f;                   //values that can be used differently by different methods
  float attrValue2 = 0.0f;                   //values that can be used differently by different methods
  float attrValue3 = 0.0f;                   //values that can be used differently by different methods

  //DATA USED FOR PARTICLE SIMULATION
  particle particles[NUM_PARTICLES]; //array of particles
  float particleTimer = 1;           //timer to determine when a new particle needs to be spawned
  pixelData pixeldata[900];          //data stored by each pixel to calculate color

  //METHODS

  //Creates a particle at the given index
  void createParticle(int pi)
  {
    particles[pi].life = 1;
    switch (posInitMethod) //define initial position
    {
    case SET:
      particles[pi].pos = posInitValue;
      break;
    case RANDOM:
      particles[pi].pos = randomFloat(450 - posInitValue, 450 + posInitValue);
      break;
    }
    switch (velInitMethod) //define initial velocity
    {
    case SET:
      particles[pi].vel = velInitValue;
      break;
    case RANDOM:
      particles[pi].vel = randomFloat(-velInitValue, velInitValue);
      break;
    }
  }

  //Calculates an attribute during particle simulation
  float calculateAttribute(float life)
  {
    switch (calcMethod)
    {
    case CONSTANT:
      return attrValue1;
    case SCALEDLIFETIME:
      return 0; //implement lol
    }
    return 0; //should never reach here
  }

  //Calculates the correct intensity of a particle based on its age
  float calculateIntensity(float life)
  {
    return life; //need to implement more interesting methods
  }

  //Calculates a multiplier for the influence a pixel feels from each particle
  float calculateDistanceModifier(float distance)
  {
    return 10 / distance;
  }

  //Calculates the color a pixel should recieve from a particle
  RgbColor calculateColor(float life, float distance)
  {
    return RgbColor(255.0 - distance * 5.0, life * 255.0, 255.0 - life * 255.0).Dim(255 - distance * 5); //test
  }

  //Calculates the radius of pixels a particle should affect
  int calculateRangeOfInfluence(float life)
  {
    return 50;
  }

  //Evaluates whether a particle needs to be spawned and will
  //correctly create a new particle in the array if needed
  void handleCreation(float delta)
  {
    particleTimer -= timerDecay * delta; //decrement our timer by the decay value
    bool shouldCreateParticle = false;   //boolean to track if a new particle needs to be created
    if (particleTimer <= 0)              //if the timer is below 0, create particle
    {
      Serial.print("Attempting particle... ");
      shouldCreateParticle = true;
      particleTimer = 1; //add one to the timer instead of setting as one because it allows for more granular control of rate
    }
    if (shouldCreateParticle)
    {
      for (int pi = 0; pi < NUM_PARTICLES; pi++) //iterate over particles
      {
        if (particles[pi].life <= 0) //particle is dead if life is less than 0, and can be replaced by a new particle
        {
          createParticle(pi); //create particle
          Serial.print("Particle Created at ");
          Serial.println(pi);
          return; //break from loop as particle has been created
        }
      }
    }
    if (shouldCreateParticle)
      Serial.println("NO ROOM");
  }

  //Calculates physics and updates all particles
  void updateParticles(float delta)
  {
    for (int pi = 0; pi < NUM_PARTICLES; pi++)
    {
      if (particles[pi].life <= 0)
        continue; //if life is less than 0, our particle is dead and deserves no consideration

      switch (calculatedAttribute) //switch based on which attribute we should calculate and which we should simulate
      {
      case ACC: //calculate acceleration and then use newtons method to approximate the integral
      {
        float acceleration = calculateAttribute(particles[pi].life);
        particles[pi].vel += acceleration * delta;
        particles[pi].pos += particles[pi].vel * delta;
        break;
      }
      case VEL: //calculate velocity and then use newtons method to approximate the integral
      {
        particles[pi].vel = calculateAttribute(particles[pi].life);
        particles[pi].pos += particles[pi].vel * delta;
        break;
      }
      case POS: //calculate position
      {
        particles[pi].pos = calculateAttribute(particles[pi].life);
        break;
      }
      }
      particles[pi].pos = fmod(particles[pi].pos, 900); //wrap particles who pass the boundaries of the simulation area

      int rangeOfInfluence = calculateRangeOfInfluence(particles[pi].life);
      particles[pi].life -= particleDecay * delta;                                                      //decrement our life
      for (int i = particles[pi].pos - rangeOfInfluence; i < particles[pi].pos + rangeOfInfluence; i++) //iterate over all pixels within the range of influence of the particle
      {
        int pixI = mod(i, 900);                      //index of pixel, particle effects will wrap around to the beginning
        float distance = abs(particles[pi].pos - i); //distance from particle to pixel
        float influence = calculateDistanceModifier(distance) * calculateIntensity(particles[pi].life);
        if (influence > pixeldata[pixI].refrences[0].influence) //if the particle has more influence than the most influential particle, shift to make room and replace
        {
          pixeldata[pixI].refrences[3] = pixeldata[pixI].refrences[2];
          pixeldata[pixI].refrences[2] = pixeldata[pixI].refrences[1];
          pixeldata[pixI].refrences[1] = pixeldata[pixI].refrences[0];
          pixeldata[pixI].refrences[0].influence = influence;
          pixeldata[pixI].refrences[0].color = calculateColor(particles[pi].life, distance);
        }
        else if (influence > pixeldata[pixI].refrences[1].influence) //if the particle has more influence than the second most influential particle, shift to make room and replace
        {
          pixeldata[pixI].refrences[3] = pixeldata[pixI].refrences[2];
          pixeldata[pixI].refrences[2] = pixeldata[pixI].refrences[1];
          pixeldata[pixI].refrences[1].influence = influence;
          pixeldata[pixI].refrences[1].color = calculateColor(particles[pi].life, distance);
        }
        else if (influence > pixeldata[pixI].refrences[2].influence) //if the particle has more influence than the third most influential particle, shift to make room and replace
        {
          pixeldata[pixI].refrences[3] = pixeldata[pixI].refrences[2];
          pixeldata[pixI].refrences[2].influence = influence;
          pixeldata[pixI].refrences[2].color = calculateColor(particles[pi].life, distance);
        }
        else if (influence > pixeldata[pixI].refrences[3].influence) //if the particle has more influence than the least influential particle, replace
        {
          pixeldata[pixI].refrences[3].influence = influence;
          pixeldata[pixI].refrences[3].color = calculateColor(particles[pi].life, distance);
        }
      }
    }
  }

  void resetParticleReference(int index)
  {
    for (int i = 0; i < 4; i++)
    {
      pixeldata[index].refrences[i].influence = 0.01;
      pixeldata[index].refrences[i].color.R = 0;
      pixeldata[index].refrences[i].color.G = 0;
      pixeldata[index].refrences[i].color.B = 0;
    }
  }

public:
  void update(float delta)
  {
    handleCreation(delta);
    updateParticles(delta);
  }

  //calculates the color for a pixel at a given index
  RgbColor calculatePixel(int index)
  {
    //prepare information to bilinear blend
    //0, 1
    //2, 3
    float rightInfluence = pixeldata[index].refrences[1].influence + pixeldata[index].refrences[3].influence;                                                                                     //total weight of the right 2 colors
    float bottomInfluence = pixeldata[index].refrences[2].influence + pixeldata[index].refrences[3].influence;                                                                                    //total weight of the bottom 2 colors
    float totalInfluence = pixeldata[index].refrences[0].influence + pixeldata[index].refrences[1].influence + pixeldata[index].refrences[2].influence + pixeldata[index].refrences[3].influence; //total influence

    RgbColor pixelColor = RgbColor::BilinearBlend(pixeldata[index].refrences[0].color, pixeldata[index].refrences[1].color,
                                                  pixeldata[index].refrences[2].color, pixeldata[index].refrences[3].color,
                                                  rightInfluence / totalInfluence, bottomInfluence / totalInfluence);

    resetParticleReference(index);
    return pixelColor;
  }
};
