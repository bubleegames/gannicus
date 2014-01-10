#include "controller.h"

keySetting::keySetting()
{
	trigger.type = 0;
	effect = 0;
}

void controller::init()
{
	inputBuffer.clear();
	for(int i = 0; i < 30; i++) inputBuffer.push_back(5);
}


script * controller::patternMatch(int effect)
{
	for(unsigned int i = 0; i < pattern.size(); i++){
		if(abs(effect) & pattern[i]) return macro[i];
	}
	return nullptr;
}

void controller::setKey(int effect)
{
	SDL_Event temp;
	bool configFlag = 0;
	while(SDL_PollEvent(&temp));
	while (!configFlag){
		if (SDL_PollEvent(&temp)) {
			for(unsigned int i = 0; i < input.size(); i++){
				if(input[i]->effect == effect) input.erase(input.begin()+i);
			}
			if(temp.type == SDL_JOYHATMOTION) {
				if(effect == 1){
					temp.jhat.value = 8;
					configFlag = setKey(effect, temp);
					temp.jhat.value = 2;
					configFlag = setKey(effect << 1, temp);
					temp.jhat.value = 1;
					configFlag = setKey(effect << 2, temp);
					temp.jhat.value = 4;
					configFlag = setKey(effect << 3, temp);
				}
			} else { 
				configFlag = setKey(effect, temp);
			}
		}
	}
}

/*This function takes an event and a desired effect and maps them in the keySetting array*/
bool controller::setKey(int effect, SDL_Event temp)
{
	int workingIndex = -1;
	switch (temp.type){
	case SDL_JOYHATMOTION:
		if(temp.jaxis.value != 0){
			for(unsigned int i = 0; i < input.size(); i++){
				if(input[i]->trigger.type == temp.type &&
				   input[i]->trigger.jhat.which == temp.jhat.which &&
				   input[i]->trigger.jhat.hat == temp.jhat.hat &&
				   input[i]->trigger.jhat.value == temp.jhat.value){
					input.erase(input.begin()+i);
					i = input.size();
				}
			}
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.jhat.which = temp.jhat.which;
			input[workingIndex]->trigger.jhat.hat = temp.jhat.hat;
			input[workingIndex]->trigger.jhat.value = temp.jhat.value;
		}
		break;
	case SDL_JOYAXISMOTION:
		if(temp.jaxis.value != 0){
			for(unsigned int i = 0; i < input.size(); i++){
				if(input[i]->trigger.type == temp.type &&
				   input[i]->trigger.jaxis.which == temp.jaxis.which &&
				   input[i]->trigger.jaxis.axis == temp.jaxis.axis &&
				   input[i]->trigger.jaxis.value == temp.jaxis.value){
					input.erase(input.begin()+i);
					i = input.size();
				}
			}
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.jaxis.which = temp.jaxis.which;
			input[workingIndex]->trigger.jaxis.axis = temp.jaxis.axis;
			input[workingIndex]->trigger.jaxis.value = temp.jaxis.value;
		}
		break;
	case SDL_JOYBUTTONDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.jbutton.which == temp.jbutton.which &&
			   input[i]->trigger.jbutton.button == temp.jbutton.button){
				input.erase(input.begin()+i);
				i = input.size();
			}
		}
		input.push_back(new keySetting);
		workingIndex = input.size() - 1;
		input[workingIndex]->trigger.type = temp.type;
		input[workingIndex]->trigger.jbutton.which = temp.jbutton.which;
		input[workingIndex]->trigger.jbutton.button = temp.jbutton.button;
		break;
	case SDL_KEYDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.key.keysym.sym == temp.key.keysym.sym){
				input.erase(input.begin()+i);
				i = input.size();
			}
		}
		input.push_back(new keySetting);
		workingIndex = input.size() - 1;
		input[workingIndex]->trigger.type = temp.type;
		input[workingIndex]->trigger.key.keysym.sym = temp.key.keysym.sym;
		break;
	default:
		break;
	}
	if(workingIndex > -1){
		input[workingIndex]->effect = effect;
		return 1;
	} else return 0;
}

void controller::pushInput(unsigned int i)
{
	inputBuffer.push_front(i);
	inputBuffer.pop_back();
}

bool controller::same(SDL_Event temp)
{
	switch (temp.type){
	case SDL_KEYDOWN:
		if(input[0]->trigger.type == temp.type) return true;
	case SDL_JOYBUTTONDOWN:
		if(input[0]->trigger.type == SDL_JOYAXISMOTION && temp.jbutton.which == input[0]->trigger.jaxis.which) return true;
		else if(input[0]->trigger.type == SDL_JOYBUTTONDOWN && temp.jbutton.which == input[0]->trigger.jbutton.which) return true;
		else if(input[0]->trigger.type == SDL_JOYHATMOTION && temp.jhat.which == input[0]->trigger.jhat.which) return true;
	case SDL_JOYAXISMOTION:
		if(input[0]->trigger.type == SDL_JOYAXISMOTION && temp.jaxis.which == input[0]->trigger.jaxis.which) return true;
		else if(input[0]->trigger.type == SDL_JOYBUTTONDOWN && temp.jaxis.which == input[0]->trigger.jbutton.which) return true;
		else if(input[0]->trigger.type == SDL_JOYHATMOTION && temp.jhat.which == input[0]->trigger.jhat.which) return true;
	case SDL_JOYHATMOTION:
		if(input[0]->trigger.type == SDL_JOYAXISMOTION && temp.jaxis.which == input[0]->trigger.jaxis.which) return true;
		else if(input[0]->trigger.type == SDL_JOYBUTTONDOWN && temp.jaxis.which == input[0]->trigger.jbutton.which) return true;
		else if(input[0]->trigger.type == SDL_JOYHATMOTION && temp.jhat.which == input[0]->trigger.jhat.which) return true;
	}
	return false;
}

void controller::swapKey(int effect, SDL_Event temp)
{
	int sameKey = -1, sameEffect = -1;
	for(unsigned int i = 0; i < input.size(); i++){
		if(input[i]->effect == effect){
			sameEffect = i;
			i = input.size();
		}
	}
	for(unsigned int i = 0; i < input.size(); i++){
		switch(temp.type){
		case SDL_KEYDOWN:
			if(input[i]->trigger.key.keysym.sym == temp.key.keysym.sym){
				sameKey = i;
				i = input.size();
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if(input[i]->trigger.jbutton.button == temp.jbutton.button){
				sameKey = i;
				i = input.size();
			}
			break;
		case SDL_JOYAXISMOTION:
			if(input[i]->trigger.jaxis.axis == temp.jaxis.axis && input[i]->trigger.jaxis.value == temp.jaxis.value){
				sameKey = i;
				i = input.size();
			}
			break;
		case SDL_JOYHATMOTION:
			if(input[i]->trigger.jhat.hat == temp.jhat.hat && input[i]->trigger.jhat.value == temp.jhat.value){
				sameKey = i;
				i = input.size();
			}
			break;
		default:
			return;
		}
	}
	if(sameEffect >= 0){
		if(sameKey >= 0){
			if(sameKey == sameEffect) return;
			input[sameEffect]->effect = input[sameKey]->effect;
			input[sameKey]->effect = effect;
		} else {
			setKey(effect, temp);
			input.erase(input.begin()+sameEffect);
		}
	} else {
		if(sameKey >= 0) input[sameKey]->effect = effect;
		else setKey(effect, temp);
	}
}

int inputHandler::tap(const SDL_Event &event)
{
	for(keySetting *i:input){
		if(i->trigger.type == event.type){
			switch(event.type){
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				if(i->trigger.key.keysym.sym == event.key.keysym.sym)
					return i->effect;
			case SDL_JOYBUTTONUP:
			case SDL_JOYBUTTONDOWN:
				if(i->trigger.jbutton.which == event.jbutton.which && 
				   i->trigger.jbutton.button == event.jbutton.button) 
					return i->effect;
			case SDL_JOYAXISMOTION:
				if(i->trigger.jaxis.which == event.jaxis.which &&
				   i->trigger.jaxis.axis == event.jaxis.axis &&
				   i->trigger.jaxis.value == event.jaxis.value)
					return i->effect;
			case SDL_JOYHATMOTION:
				if(i->trigger.jhat.which == event.jhat.which &&
				   i->trigger.jhat.hat == event.jhat.hat &&
				   i->trigger.jhat.value == event.jhat.value)
					return i->effect;
			default:
				break;
			}
		}
	}
	return 0;
}

int controller::tap(const SDL_Event &event){
{
	int ret = 0;
	for(keySetting *i:input){
		switch(event.type){
		case SDL_KEYUP:
			if(i->trigger.type == SDL_KEYDOWN){
				if(i->trigger.key.keysym.sym == event.key.keysym.sym) 
					ret -= i->effect;
			}
			break;
		case SDL_KEYDOWN:
			if(i->trigger.type == SDL_KEYDOWN){
				if(i->trigger.key.keysym.sym == event.key.keysym.sym) 
					ret += i->effect;
			}
			break;
		case SDL_JOYBUTTONUP:
			if(i->trigger.type == SDL_JOYBUTTONDOWN){
				if(i->trigger.jbutton.which == event.jbutton.which && 
				   i->trigger.jbutton.button == event.jbutton.button) 
					ret -= i->effect;
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if(i->trigger.type == SDL_JOYBUTTONDOWN){
				if(i->trigger.jbutton.which == event.jbutton.which && 
				   i->trigger.jbutton.button == event.jbutton.button) 
					ret += i->effect;
			}
			break;
		case SDL_JOYAXISMOTION:
			if(i->trigger.type == SDL_JOYAXISMOTION){
				if(i->trigger.jaxis.which == event.jaxis.which &&
				   i->trigger.jaxis.axis == event.jaxis.axis){
					if(i->trigger.jaxis.value == event.jaxis.value){
						ret += i->effect;
					} else {
						if(event.jaxis.value == 0) ret -= i->effect;
					}
				}
			}
			break;
		default:
			inputHandler::tap(event);
		}
	}
	return ret;
}
}
