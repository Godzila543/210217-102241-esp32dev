#define NUM_PARTICLES 300
//atribute initiation method, used to determine the initial positions, velocities, and accelerations of particles
enum class AttrInitMethod
{
  SET,
  RANDOM
};
//attributes can either calculate based on lifetime or can be calculated from the next derivative down
enum class AttrCalcMethod
{
  CONSTANT,
  SCALEDLIFETIME,
  ATTRACTOR
};
//determines which attribute should be calculated and which should be determined from derivatives
enum class DerivativeLevel
{
  POS,
  VEL,
  ACC
};
//determines how intensity is calculated throughout the life of a particle
enum class IntensityMethod
{
  FADEOUT,
  FADEIN,
  FADEINOUT,
  PULSE
};
//determines how color is chosen from the palette
enum class ColorMethod
{
  LIFE,
  INTENSITY,
  INFLUENCE,
  SET
};

//A data structure for holding information relevant to particles
struct Particle
{
  float life = 0; //particles will have a life from 1 to 0
  float pos;
  float vel;
  float color;
};

//a fog which controls the defualt color and influence of a reference
struct Fog
{
  RgbColor color = RgbColor(0, 0, 0);
  float influence = 3;
};

//A reference to a specific particle with information used to calculate the color
struct ParticleReference
{
  RgbColor color = RgbColor(0, 0, 0);
  float influence = 0.01;
};

//Holds all the refrences for a given pixel required to calculate the final color
struct PixelData
{
  ParticleReference refrences[4];
};

//Struct which holds configuration information for a ParticleGenerator
struct ParticleSettings
{
  float particleDecay = 0.025; //decay will be subtracted from particle life[1, 0] every iteration
  float timerDecay = 0.40;     //a timer in the range of [1, 0] will be subtracted by this

  IntensityMethod intensityMethod = IntensityMethod::FADEINOUT; //holds the method to be used to calculate the intensity over the life of a particle
  ColorMethod colorMethod = ColorMethod::LIFE;                  //holds the method used to calculate the color of a particle
  float peakRange = 10;                                         //Determines the distance where the modifier = 0
  float edgeValue = 0.005;                                      //The lowest influence on a particle at maximum intensity
  float influencePreCalc;
  Fog fog;                                                      //Holds the default intensity and color of a refernce

  AttrInitMethod posInitMethod = AttrInitMethod::RANDOM; //Initiaion method for position attribute of particles
  AttrInitMethod velInitMethod = AttrInitMethod::RANDOM; //Initiaion method for velocity attribute of particles
  float posInitValue1 = 0.0;                             //Range/value of position
  float posInitValue2 = 900.0;                           //Range/value of position
  float velInitValue1 = -1.0;                            //Range/value of velocity
  float velInitValue2 = 1.0;

  DerivativeLevel calculatedAttribute = DerivativeLevel::ACC; //The attribute of the particle to calculate, which will be integrated to find pos/vel
  AttrCalcMethod calcMethod = AttrCalcMethod::CONSTANT;       //method to calculate value of calculatedAttribute
  float attrValue1 = 0.0;                                     //values that can be used differently by different methods
  float attrValue2 = 0.0;                                     //values that can be used differently by different methods
  float attrValue3 = 0.0;                                     //values that can be used differently by different methods

};

//generator for determining pixel colors based on a simulated particle system
class ParticleGenerator : Generator
{
  //ATTRIBUTES TO BE CUSTOMIZED

  Palette palette; //FIXME maybe find better solution

  float particleDecay; //decay will be subtracted from particle life[1, 0] every iteration
  float timerDecay;     //a timer in the range of [1, 0] will be subtracted by this

  IntensityMethod intensityMethod; //holds the method to be used to calculate the intensity over the life of a particle
  ColorMethod colorMethod;                  //holds the method used to calculate the color of a particle
  float peakRange;                                         //Determines the distance where the modifier = 0
  float edgeValue;                                      //The lowest influence on a particle at maximum intensity
  float influencePreCalc;
  Fog fog;                                                      //Holds the default intensity and color of a refernce

  AttrInitMethod posInitMethod; //Initiaion method for position attribute of particles
  AttrInitMethod velInitMethod; //Initiaion method for velocity attribute of particles
  float posInitValue1;                             //Range/value of position
  float posInitValue2;                           //Range/value of position
  float velInitValue1;                            //Range/value of velocity
  float velInitValue2;

  DerivativeLevel calculatedAttribute; //The attribute of the particle to calculate, which will be integrated to find pos/vel
  AttrCalcMethod calcMethod;       //method to calculate value of calculatedAttribute
  float attrValue1;                                     //values that can be used differently by different methods
  float attrValue2;                                     //values that can be used differently by different methods
  float attrValue3;                                     //values that can be used differently by different methods

  //DATA USED FOR PARTICLE SIMULATION
  Particle particles[NUM_PARTICLES]; //array of particles
  float particleTimer = 1;           //timer to determine when a new particle needs to be spawned
  PixelData pixeldata[900];          //data stored by each pixel to calculate color

  //CONSTRUCTOR
  ParticleGenerator(ParticleSettings settings = ParticleSettings())
  {
    particleDecay = settings.particleDecay;
    timerDecay = settings.timerDecay;
    intensityMethod = settings.intensityMethod;
    colorMethod = settings.colorMethod;
    peakRange = settings.peakRange;
    edgeValue = settings.edgeValue;
    fog = settings.fog;
    posInitMethod = settings.posInitMethod;
    velInitMethod = settings.velInitMethod;
    posInitValue1 = settings.posInitValue1;
    posInitValue2 = settings.posInitValue2;
    velInitValue1 = settings.velInitValue1;
    velInitValue2 = settings.velInitValue2;
    calculatedAttribute = settings.calculatedAttribute;
    calcMethod = settings.calcMethod;
    attrValue1 = settings.attrValue1;
    attrValue2 = settings.attrValue2;
    attrValue3 = settings.attrValue3;

    influencePreCalc = peakRange * peakRange * edgeValue;
  }
  
  //METHODS

  //Creates a particle at the given index
  void createParticle(int pi)
  {
    particles[pi].life = 1;
    switch (posInitMethod) //define initial position
    {
    case AttrInitMethod::SET:
      particles[pi].pos = posInitValue1;
      break;
    case AttrInitMethod::RANDOM:
      particles[pi].pos = randomFloat(posInitValue1, posInitValue2);
      break;
    }
    switch (velInitMethod) //define initial velocity
    {
    case AttrInitMethod::SET:
      particles[pi].vel = velInitValue1;
      break;
    case AttrInitMethod::RANDOM:
      particles[pi].vel = randomFloat(velInitValue1, velInitValue2);
      break;
    }
    if (colorMethod == ColorMethod::SET)
      particles[pi].color = randomFloat(0, 1);
  }

  //Calculates an attribute during particle simulation
  float calculateAttribute(float life, float attribute)
  {
    switch (calcMethod)
    {
    case AttrCalcMethod::CONSTANT:
      return attrValue1;
    case AttrCalcMethod::SCALEDLIFETIME:
      return (attrValue2 - attrValue1) * life + attrValue1;
    case AttrCalcMethod::ATTRACTOR:
      return attribute + (attrValue1 - attribute) * attrValue2;
    }
    return 0; //should never reach here
  }

  //Calculates the correct intensity of a particle based on its age
  float calculateIntensity(float life)
  {
    switch (intensityMethod)
    {
    case IntensityMethod::FADEOUT:
      return life;
    case IntensityMethod::FADEIN:
      return 1 - life;
    case IntensityMethod::FADEINOUT:
      return 1 - abs(1 - 2 * life);
    case IntensityMethod::PULSE:
      return life; //TODO pulsate (sin?)
    }
    return 0; //failsafe
  }

  //TODO create custom methods
  //Calculates a multiplier for the influence a pixel feels from each particle from [0, 1]
  float calculateDistanceModifier(float distance)
  {
    float modifier = influencePreCalc / (distance*distance+influencePreCalc);
    return modifier - edgeValue;
  }

  //Calculates the color a pixel should recieve from a particle
  RgbColor calculateColor(Particle p, float distance)
  {
    float intensity = calculateIntensity(p.life);
    float influence = intensity * calculateDistanceModifier(distance);
    switch (colorMethod)
    {
    case ColorMethod::LIFE:
      return palette.getColor(1 - p.life);
    case ColorMethod::INFLUENCE:
      return palette.getColor(constrain(influence, 0, 1));
    case ColorMethod::INTENSITY:
      return palette.getColor(intensity);
    case ColorMethod::SET:
      return palette.getColor(p.color);
    }

    return RgbColor(255, 255, 255); //execution should not reach here
  }

  //TODO enhance this function to account for other shtuff
  //Calculates the radius of pixels a particle should affect
  int calculateRangeOfInfluence(float intensity)
  {
    //float averageParticles = timerDecay / particleDecay; //average number of particles at any time
    //float averageDistance = 900 / averageParticles;      //average distance between particles
    return sqrt((influencePreCalc*intensity)/edgeValue);
  }

  //Evaluates whether a particle needs to be spawned and will
  //correctly create a new particle in the array if needed
  void handleCreation(float delta)
  {
    particleTimer -= timerDecay * delta; //decrement our timer by the decay value
    int particlesToCreate = 0;           //tracks how many particles need to be created
    if (particleTimer <= 0)              //if the timer is below 0, create particles
    {
      particlesToCreate = abs(floor(particleTimer));
      particleTimer += particlesToCreate; //add the number of particles needed
    }
    if (particlesToCreate > 0)
    {
      for (int pi = 0; pi < NUM_PARTICLES; pi++) //iterate over particles
      {
        if (particles[pi].life <= 0) //particle is dead if life is less than 0, and can be replaced by a new particle
        {
          createParticle(pi); //create particle
          particlesToCreate -= 1;
          if (particlesToCreate == 0)
            return; //break from loop as particle has been created
        }
      }
    }
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
      case DerivativeLevel::ACC: //calculate acceleration and then use newtons method to approximate the integral
      {
        float acceleration = calculateAttribute(particles[pi].life, 0);
        particles[pi].vel += acceleration * delta;
        particles[pi].pos += particles[pi].vel * delta;
        break;
      }
      case DerivativeLevel::VEL: //calculate velocity and then use newtons method to approximate the integral
      {
        particles[pi].vel = calculateAttribute(particles[pi].life, particles[pi].vel);
        particles[pi].pos += particles[pi].vel * delta;
        break;
      }
      case DerivativeLevel::POS: //calculate position
      {
        particles[pi].pos = calculateAttribute(particles[pi].life, particles[pi].pos);
        break;
      }
      }
      //FIXME fmod is bad for negative values
      particles[pi].pos = fmod(particles[pi].pos, 900); //wrap particles who pass the boundaries of the simulation area

      float intensity = calculateIntensity(particles[pi].life);
      int rangeOfInfluence = calculateRangeOfInfluence(intensity);
      particles[pi].life -= particleDecay * delta;                                                      //decrement our life
      for (int i = particles[pi].pos - rangeOfInfluence; i < particles[pi].pos + rangeOfInfluence; i++) //iterate over all pixels within the range of influence of the particle
      {
        int pixI = mod(i, 900);                      //index of pixel, particle effects will wrap around to the beginning
        float distance = abs(particles[pi].pos - i); //distance from particle to pixel
        float influence = calculateDistanceModifier(distance) * intensity;
        if (influence > pixeldata[pixI].refrences[0].influence) //if the particle has more influence than the most influential particle, shift to make room and replace
        {
          pixeldata[pixI].refrences[3] = pixeldata[pixI].refrences[2];
          pixeldata[pixI].refrences[2] = pixeldata[pixI].refrences[1];
          pixeldata[pixI].refrences[1] = pixeldata[pixI].refrences[0];
          pixeldata[pixI].refrences[0].influence = influence;
          pixeldata[pixI].refrences[0].color = calculateColor(particles[pi], distance);
        }
        else if (influence > pixeldata[pixI].refrences[1].influence) //if the particle has more influence than the second most influential particle, shift to make room and replace
        {
          pixeldata[pixI].refrences[3] = pixeldata[pixI].refrences[2];
          pixeldata[pixI].refrences[2] = pixeldata[pixI].refrences[1];
          pixeldata[pixI].refrences[1].influence = influence;
          pixeldata[pixI].refrences[1].color = calculateColor(particles[pi], distance);
        }
        else if (influence > pixeldata[pixI].refrences[2].influence) //if the particle has more influence than the third most influential particle, shift to make room and replace
        {
          pixeldata[pixI].refrences[3] = pixeldata[pixI].refrences[2];
          pixeldata[pixI].refrences[2].influence = influence;
          pixeldata[pixI].refrences[2].color = calculateColor(particles[pi], distance);
        }
        else if (influence > pixeldata[pixI].refrences[3].influence) //if the particle has more influence than the least influential particle, replace
        {
          pixeldata[pixI].refrences[3].influence = influence;
          pixeldata[pixI].refrences[3].color = calculateColor(particles[pi], distance);
        }
      }
    }
  }

  void resetParticleReference(int index)
  {
    for (int i = 0; i < 4; i++)
    {
      pixeldata[index].refrences[i].influence = fog.influence / 3;
      pixeldata[index].refrences[i].color = fog.color;
    }
  }

public:
  void setPalette(Palette pal)
  {
    palette = pal;
  }
  void setPreset(float pDecay, float tDecay, int iMethod, int cMethod, float range, float fStrength, RgbColor fColor, int pMethod, int vMethod, float pV1, float pV2, float vV1, float vV2, int cAttribute, int caMethod, float aV1, float aV2, float aV3)
  {
    particleDecay = pDecay;
    timerDecay = tDecay;

    intensityMethod = (IntensityMethod)iMethod;
    colorMethod = (ColorMethod)cMethod;
    distanceScalar = range;
    fog.influence = fStrength;
    fog.color = fColor;

    posInitMethod = (AttrInitMethod)pMethod;
    velInitMethod = (AttrInitMethod)vMethod;
    posInitValue1 = pV1;
    posInitValue2 = pV2;
    velInitValue1 = vV1;
    velInitValue2 = vV2;

    calculatedAttribute = (DerivativeLevel)cAttribute;
    calcMethod = (AttrCalcMethod)caMethod;
    attrValue1 = aV1;
    attrValue2 = aV2;
    attrValue3 = aV3;

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
      particles[i].life = 0;
    }
  }
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
