#include <ncurses.h>
#include <cmath>
#include <panel.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cstring>
#include <chrono>
#include <thread>
#include <cstdlib>

class pipe {
public:
	int middle_y;
	int space_x;
	std::vector<std::string> pixelbuffer;
	int pixel_offset;
	void init_pipe( int lines ){
		this->middle_y = (std::rand() % (lines - 30)) + 15;
		this->space_x = 50;
		this->pixel_offset = 0;
		
		int pipe_offset_top = 0;
		int pipe_offset_bottom = 0;
		for ( int y = 0; y < lines; y++ ){
			//draw top of pipe
			if (y < middle_y - 9){
				this->pixelbuffer.push_back("  # I   x   #  ");
			}
			else if (y < middle_y - 5){
				switch (pipe_offset_top){
				case 0:
					this->pixelbuffer.push_back("###############");
					break;
				case 1:	
					this->pixelbuffer.push_back("#  I      x   #");
					break;
				case 2:
					this->pixelbuffer.push_back("#  I      x   #");
					break;
				case 3:
					this->pixelbuffer.push_back("###############");
				}
				pipe_offset_top += 1;
				if (pipe_offset_top > 3){
					pipe_offset_top = 0;
				}
			}
			//draw middle
			else if (y >= middle_y - 5 and y <= middle_y + 5) {
				this->pixelbuffer.push_back("               ");
			}
			//draw bottom of pipe
			else if (y > middle_y + 5 + 4){
				this->pixelbuffer.push_back("  # I   x   #  ");
			}
			else if (y > middle_y + 5) {
				switch (pipe_offset_bottom){
				case 0:
					this->pixelbuffer.push_back("###############");
					break;
				case 1:	
					this->pixelbuffer.push_back("#  I      x   #");
					break;
				case 2:
					this->pixelbuffer.push_back("#  I      x   #");
					break;
				case 3:
					this->pixelbuffer.push_back("###############");
				}
				pipe_offset_bottom += 1;
				if (pipe_offset_bottom > 3){
					pipe_offset_bottom = 0;
				}
			}
			else{
				this->pixelbuffer.push_back("               ");
			}
			
		}
	}
};

class bird {
public:
	int bird_x;
	int bird_y;
	float fbird_dy;
	float fbird_y;
	bool alive;
	int score;
	std::vector<std::string> pixelbuffer;
	void init_bird(){
		this->bird_x = 10;
		this->alive = true;
		this->score = 0;
		this->bird_y = 10;
		this->fbird_dy = 0.0f;
		this->fbird_y = 10.0f;
		this->pixelbuffer.push_back(" '_' ");
		this->pixelbuffer.push_back("( O< ");
		this->pixelbuffer.push_back(" ( ( ");
		this->pixelbuffer.push_back("/ \"\" ");
		this->pixelbuffer.push_back("     ");
	};
	void do_gravity(float gravity_constant){
		this->fbird_dy += gravity_constant;
		this->fbird_y += fbird_dy;
		this->bird_y = static_cast<int>(std::round(fbird_y));
	}
};

void draw_frame(int columns, int lines, int bird_x, int bird_y, bird Bird, std::vector<pipe> pipes){
	
	std::string linebuffer = "";
	int bird_offset = 0;
	int bird_offset_y = 0;
	bool drawingPipe = false;
	for (int y = 0; y < lines-4; y++){
		
		for (int p = 0; p < pipes.size(); p++){
			pipes[p].pixel_offset = 0;
		}
		bird_offset = 0;
		
		if (y >= bird_y && y < bird_y + 4){
			bird_offset_y = y - bird_y;
		}

		for (int x = 0; x < columns; x++){
			
			//check if we are in a bird bounding box
			if (y >= bird_y && y < bird_y + 4 && x >= bird_x && x < bird_x + 4){
				
				//linebuffer.append("#");
				linebuffer.append(Bird.pixelbuffer[bird_offset_y].substr(bird_offset,1));
				bird_offset += 1;
				if (bird_offset > 4){
					bird_offset = 0;
				}

			} else {
				//check if we are in a pipe
				drawingPipe = false;
				for (int p = 0; p < pipes.size(); p++){
					if ( x >= pipes[p].space_x && x < pipes[p].space_x + 15 && pipes[p].space_x < columns - 18){
						//char *letter = &pipes[p].pixelbuffer[y].at(pipes[p].pixel_offset);
						//std::string sletter = letter;
						
						//linebuffer.append(sletter);
						//linebuffer.append("3");
						linebuffer.append(pipes[p].pixelbuffer[y].substr(pipes[p].pixel_offset,1));
						pipes[p].pixel_offset += 1;
						if( pipes[p].pixel_offset > 14){
							pipes[p].pixel_offset = 0;
						}

						drawingPipe = true;
					}
				}
				if (!drawingPipe){
					linebuffer.append(" ");
				}

			}


		}
		char* buff = new char[linebuffer.length() + 1];
		std::strcpy(buff, linebuffer.c_str());
		addstr(buff);
		linebuffer = "";
	}

	for (int y = 0; y < 4; y++){
		if (y < 3 ) {
			for ( int x = 0; x < columns; x++){
				linebuffer.append("~");	
			}
			char* buff = new char[linebuffer.length() + 1];
			std::strcpy(buff, linebuffer.c_str());
			addstr(buff);
			linebuffer = "";
		} else {
			std::string score = "score: " + std::to_string(Bird.score);
			char* buff = new char[score.length()+1];
			std::strcpy(buff, score.c_str());
			addstr(buff);
		}

	}

	refresh();
	return;
}

bool hit_check(bird Bird, std::vector<pipe> pipes, int lines){
	bool hit = false;
	for (int p=0; p<pipes.size(); p++){
		if ( Bird.bird_y > pipes[p].middle_y+6 || Bird.bird_y < pipes[p].middle_y-4 ) {
			if (Bird.bird_x + 4 > pipes[p].space_x && Bird.bird_x < pipes[p].space_x + 15) {
				hit = true;
			}
		}
	}
	
	if (Bird.bird_y >= lines-10){
		hit = true;
	}
	return hit;

}

int get_score(bird Bird, std::vector<pipe> pipes){
	int score = 0;
	for (int p = 0; p < pipes.size(); p++) {
		if (Bird.bird_x > pipes[p].space_x){
			score++;
		}
	}
	return score;
}

int main(){
	// initialize the screen
	initscr();
	int columns, lines;
	getmaxyx(stdscr, lines, columns);
	
	//initialize the bird
	bird Bird;
	Bird.init_bird();
	//Bird.init_bird();

	//initialize the pipes
	std::vector<pipe> pipes;
	int pipe_offset = 0;
	for (int i=1; i<50; i++){
		pipe newPipe;
		newPipe.init_pipe( lines );
		newPipe.space_x += pipe_offset;
		pipe_offset += 50;
		pipes.push_back(newPipe);
	}

	//main loop

	//addstr("swag ");
	
	nodelay(stdscr, true);
	int bird_timer = 0;
	while (true){
		//handle bird physics
		int key = getch();
		if ( key == 32 && Bird.alive){
			Bird.fbird_dy = -1.0f;
		}

		if (Bird.bird_y >= lines-10){
			Bird.bird_y = lines - 10;
			Bird.fbird_y = float(lines - 10);
		}
		
		Bird.do_gravity(0.1f);

		if (hit_check(Bird, pipes, lines)){
			Bird.alive = false;
		}
		if (!Bird.alive) {
			bird_timer += 1;
		}
		if (bird_timer > 100){
			break;
		}
		Bird.score = get_score(Bird, pipes);
		
		//handle pipe movement
		for (int p=0;p<pipes.size();p++){
			if (Bird.alive) {
				pipes[p].space_x -= 1;
			}
		}

		// handle rendering
		clear();
		draw_frame(columns, lines, Bird.bird_x, Bird.bird_y, Bird,  pipes);
		refresh();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}	
	endwin();
	printf("thanks for palying crappy bird\n");
	if ( Bird.score >= 50) {
		printf("you won lol!\n");
	}
	printf("final score: %u \n", Bird.score);
	return 0;
}
