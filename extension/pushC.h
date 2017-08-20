#if defined(__GNUC__) && !defined(__clang__)
#include "string.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void PushEnvironment;
typedef void PushCode;
typedef void PushCustomInstruction;
typedef void PushName;

struct PushVector {
	double x;
	double y;
	double z;
};

typedef struct PushVector PushVector;

// creating and manipulating push environments 

PushEnvironment *pushEnvironmentNew(int seed);
void pushEnvironmentFree(PushEnvironment *environment);

void pushEnvironmentReadConfigFile(PushEnvironment *environment, char *file);

PushCustomInstruction *pushAddCallbackInstruction(PushEnvironment *environment, unsigned int (*callback)(PushEnvironment *environment, void *d), void (*freeData)(void *d), char *name, void *callbackData);
void pushAddMacroInstruction(PushEnvironment *env, char *name, PushCode *value);

int pushRun(PushEnvironment *environment, PushCode *code, int steps);
int pushStep(PushEnvironment *environment, int steps);

char *pushGetConfigString(PushEnvironment *environment);
char *pushGetStacksString(PushEnvironment *environment);
char *pushGetExecStackString(PushEnvironment *environment);

void pushClearStacks(PushEnvironment *environment);

void pushEnvironmentSetListLimit(PushEnvironment *environment, int limit);
int pushEnvironmentGetEvalPushLimit(PushEnvironment *env, int limit);

// working with push code

PushCode *pushParse(char *string);
char *pushCodeGetString(PushCode *code);
void pushCodeFree(PushCode *code);
int pushCodeSize(PushCode *code);

// manipulating the stacks

int pushIntStackSize(PushEnvironment *environment);
void pushIntStackPop(PushEnvironment *environment);
int pushIntStackTop(PushEnvironment *environment);
void pushIntStackPush(PushEnvironment *environment, int value);

int pushBoolStackSize(PushEnvironment *environment);
void pushBoolStackPop(PushEnvironment *environment);
int pushBoolStackTop(PushEnvironment *environment);
void pushBoolStackPush(PushEnvironment *environment, int value);

int pushFloatStackSize(PushEnvironment *environment);
void pushFloatStackPop(PushEnvironment *environment);
double pushFloatStackTop(PushEnvironment *environment);
void pushFloatStackPush(PushEnvironment *environment, double value);

int pushNameStackSize(PushEnvironment *environment);
void pushNameStackPop(PushEnvironment *environment);
PushName *pushNameStackTop(PushEnvironment *environment);
void pushNameStackPush(PushEnvironment *environment, PushName *value);

int pushCodeStackSize(PushEnvironment *environment);
void pushCodeStackPop(PushEnvironment *environment);
PushCode *pushCodeStackTop(PushEnvironment *environment);
void pushCodeStackPush(PushEnvironment *environment, PushCode *value);

int pushVectorStackSize(PushEnvironment *environment);
void pushVectorStackPop(PushEnvironment *environment);
PushVector *pushVectorStackTop(PushEnvironment *environment, PushVector *value);
void pushVectorStackPush(PushEnvironment *environment, PushVector *value);

PushCode *pushCodeSubtreeMutate(PushEnvironment *environment, PushCode *p1, int size);
PushCode *pushCodeCrossover(PushEnvironment *environment, PushCode *p1, PushCode *p2);
PushCode *pushCodeDeletionMutate(PushCode *p1);
PushCode *pushCodeFlattenMutate(PushCode *c);

PushCode *pushCodeRandom(PushEnvironment *environment, int size);



#ifdef __cplusplus
}
#endif /* __cplusplus */
