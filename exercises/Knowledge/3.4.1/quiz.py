import curses
from curses.textpad import Textbox, rectangle
import re
import yaml
import json
import hashlib
import pathlib

ENVIRON_KEY_FILE = "QUIZ_FILE"

Q_TYPE_KEY_TF = "TF"
Q_TYPE_KEY_FIB = "FIB"
Q_TYPE_KEY_MC = "MC"
Q_TYPE_KEY_SA = "SA"
Q_TYPE_KEY_MA = "MA"

Q_TYPES = {
    Q_TYPE_KEY_TF : "true/false",
    Q_TYPE_KEY_FIB : "fill in the blank",
    Q_TYPE_KEY_MC : "multiple choice",
    Q_TYPE_KEY_SA  : "short answer",
    Q_TYPE_KEY_MA : "multiple answer"
}

MODE_CURSES = "curses"
MODE_GRADE = "grade"
MODE_PLAINTEXT = "plain"

DETAIL_NO_RESPONSES  = 0
DETAIL_WITH_RESPONSES = 1
DETAIL_WITH_RESPONSES_AND_GRADES = 2


#cursor visbility reference: https://docs.python.org/3/library/curses.html
CURSOR_VISIBLE = 1
CURSOR_INVISIBLE = 0
CURSOR_VERY_VISIBLE = 2

def dbg(*args, **kwargs):
    print(*args, **kwargs)



""" 
Check for save file, load the save file
If already answered, skip
otherwise, ask the question, save
export result

Will save questions in save_dir as a json 
Supports curses mode only right now. accepting MRs for ascii-mode
"""

class question:

    TEMPLATE = "Question Type: {}\n\nQuestion: {}\n\n"

    def get_curses_choice(self,choices,multi_select=False):

        CURSOR_PAD = 2
        INDENT_PAD = 4

        MIN_POS = 0
        MAX_POS = len(choices) - 1
        CUR_POS = 0
        curses.curs_set(CURSOR_VERY_VISIBLE)
        max_x, max_y = self.parent.get_max_xy()

        index = 0

        cur_y,cur_x = self.stdscr.getyx()
        self.stdscr.keypad(True)

        local_max_x = max_x - cur_x

        

        for choice in choices:
            choice = str(choice)
            self.stdscr.addstr( cur_y + index, cur_x + CURSOR_PAD, choice[: local_max_x ])
            index += 2
            while len(choice) >  local_max_x :
                choice = choice[ local_max_x :]
                self.stdscr.addstr( cur_y + index, cur_x + INDENT_PAD, choice[:local_max_x])
                index += 1
        self.stdscr.move(cur_y,cur_x)
        if multi_select:
            ms_options = [False]* (MAX_POS+1)
            toggle_mark = { False : " ", True : "X"}
        while True:
            key = self.stdscr.getch()
            if key == curses.KEY_UP:
                if CUR_POS > MIN_POS:
                    CUR_POS -= 1
                    cur_y -= 2
            elif key == curses.KEY_DOWN:
                if CUR_POS < MAX_POS:
                    CUR_POS += 1
                    cur_y += 2
            elif multi_select and key == 0x20:
               ms_options[CUR_POS] = not ms_options[CUR_POS]
               self.stdscr.addch(cur_y,cur_x,toggle_mark[ms_options[CUR_POS]])
            elif key == curses.KEY_BREAK or key == 0xa:
                break
            self.stdscr.move(cur_y,cur_x)
            self.stdscr.refresh()
        if multi_select:
            return [index for index,item in enumerate(ms_options) if item == True ]
        else:
            return CUR_POS,choices[CUR_POS]



    def get_choice(self,choices,multi_select=False):
        if self.mode == MODE_CURSES:
            return self.get_curses_choice(choices,multi_select)



    def wrap_choice(self,template,choices, multi_select=False):
        self.parent.show_message(template)
        resp = self.get_choice(choices,multi_select)
        self.stdscr.clear()
        self.parent.show_message(template + "\n Got Response: {}\n".format(resp))
        return resp

    def wrap_long_input(self,template,width=40,height=1):
        template += "\nPress Ctrl+G when you are finished typing\n\n"
        self.stdscr.clear()
        self.parent.show_message(template)
        
        resp = self.get_curses_long_input(width,height)
        self.stdscr.clear()
        self.parent.show_message(self.template + "\n Got Response: \n{}\n".format(resp))

        return resp


    def get_curses_long_input(self,width=40,height=1):
        cur_y,cur_x = self.stdscr.getyx()
        
        box_width = width
        box_height = height

        box_upper_left_x = max(0, cur_x - int(box_width/2))
        box_upper_left_y = cur_y

        box_lower_right_x = box_upper_left_x + box_width
        box_lower_right_y = box_upper_left_y + box_height


        cur_y,cur_x = self.stdscr.getyx()
        #taken from python3 curses documentation https://docs.python.org/3/howto/curses.html
        editwin = curses.newwin(box_height,box_width, box_upper_left_y,box_upper_left_x)
        rectangle(self.stdscr, box_upper_left_y-1 ,box_upper_left_x-1, box_lower_right_y, box_lower_right_x+1)
        self.stdscr.refresh()

        box = Textbox(editwin)

        # Let the user edit until Ctrl-G is struck.
        box.edit()

        # Get resulting contents
        message = box.gather()
        return message



    def handle_true_false(self):
        choices = ("True","False")
        if not self.mode == MODE_GRADE:
            self.q_resp = str(self.wrap_choice(self.template,choices)[1]).lower()
        if self.q_soln != False:
            self.q_correct = self.q_resp.lower() == self.q_soln.lower()
        

    def handle_fill_in_the_blank(self):
        
        if not self.mode == MODE_GRADE:
            self.q_resp = self.wrap_long_input(self.template)

        if self.q_soln != False:
            self.q_correct = re.search(self.q_soln,self.q_resp) != None
        

    def handle_multiple_choice(self):
        if not self.mode == MODE_GRADE:
            self.q_resp = self.wrap_choice(self.template,self.q_options)
            self.q_resp = self.q_resp[0]

        if self.q_soln != None:
            self.q_correct = self.q_resp == self.q_soln

    def handle_multiple_answer(self):

        if not self.mode == MODE_GRADE:
            self.q_resp = self.wrap_choice(self.template,self.q_options,multi_select=True)

        if self.q_soln != False:
            self.q_correct = set(self.q_resp) == set(self.q_soln)

    def handle_short_answer(self):
        
        if not self.mode == MODE_GRADE:
            self.q_resp = self.wrap_long_input(self.template,width=80,height=10)

        if self.q_soln:
            self.q_correct = re.search(self.q_soln,self.q_resp) != None



    def ask_question(self):
        if self.mode == MODE_CURSES:
            self.stdscr = self.parent.stdscr
            self.template = self.TEMPLATE.format(Q_TYPES[self.q_type],self.q_text)
            self.stdscr.clear()
        if self.q_type == Q_TYPE_KEY_TF:
            self.handle_true_false()
        elif self.q_type ==Q_TYPE_KEY_FIB:
            self.handle_fill_in_the_blank()
        elif self.q_type == Q_TYPE_KEY_MC:
            self.handle_multiple_choice()
        elif self.q_type == Q_TYPE_KEY_SA:
            self.handle_short_answer()
        elif self.q_type == Q_TYPE_KEY_MA:
            self.handle_multiple_answer()
        else:
            self.dbg("unsupported question type: {}".format(self.q_type))
        if self.mode != MODE_GRADE:
            self.stdscr.getkey()
            self.save_to_file()

    def export(self,detail=DETAIL_WITH_RESPONSES_AND_GRADES):

        resp = { 
                "type"  : self.q_type,
                "text"  : self.q_text,
                "choices" : self.q_options
                }
        if detail >= DETAIL_WITH_RESPONSES:
            resp["resp"] = self.q_resp
        if detail >= DETAIL_WITH_RESPONSES_AND_GRADES:
            resp["soln"] = self.q_soln
            resp["pass"] = self.q_correct
        return resp

    def do_import(self,obj):
        self.q_type = obj["type"]
        self.q_text = obj["text"]
        self.update_resp(obj["resp"])
        if "pass" in obj.keys():
            self.q_correct = obj["pass"]
        if "soln" in obj.keys():
            self.q_soln = obj["soln"]
        if "choices" in obj.keys():
            self.q_options = obj["choices"]

    def update_resp(self,resp):
        self.q_resp = resp

    def load_from_file(self):
        if self.save_file:
            if self.save_file.exists() and self.save_file.is_file():
                try:
                    save_text = self.save_file.read_text()
                    save_obj = json.loads(save_text)
                    self.do_import(save_obj)
                except Exception as e:
                    print("Failed to load from save file: {}\nError: {}".format(str(self.save_file), str(e)))

    def save_to_file(self):
        obj = self.export()
        obj_text = json.dumps(obj)
        if self.save_file:
            self.save_file.write_text(obj_text)

    def __init__(self, q_type, q_text, parent, q_options=[], solution=False, mode=MODE_CURSES):
        self.q_type = q_type
        self.q_text = q_text
        self.q_resp = ""
        self.q_options = q_options
        self.q_soln = solution
        self.q_correct = False
        self.dbg = dbg
        self.mode = mode
        self.parent = parent
        self.save_file = False

        fn = self.q_text+self.q_type
        fn = fn.encode("utf-8")
        self.fn = "question_{}.json".format(hashlib.md5(fn).hexdigest())
        if self.parent.save_dir:
            self.save_file = self.parent.save_dir / self.fn
            
        self.load_from_file()





""" 
Does the quiz
Initialize quiz
add questions
run quiz
export results

Will save questions in save_dir
Supports curses mode only right now. accepting MRs for ascii-mode
"""
class quiz:

    WELCOME_SCREEN = "Welcome to Quiz\nPress any key to begin.\n"
    X_MARGIN = 2
    Y_MARGIN = 2

    def get_max_xy(self):
        max_y, max_x = self.stdscr.getmaxyx()
        max_y -= 2*self.Y_MARGIN
        max_x -= 2*self.X_MARGIN
        return max_x, max_y

    def show_curses_message(self,message):
        line_x = len(message.split("\n"))
        index = 0
        max_width, max_height = self.get_max_xy() 
        start_y = int( (max_height - line_x)  / 2 )

        for line in message.split("\n"):
            excess = 1
            while excess > 0:
                cur_line = line[:max_width]
                
                start_x = int((max_width - len(cur_line) + (2 * self.Y_MARGIN ))/2)

                self.stdscr.addstr( start_y + index, start_x , line[:max_width])
                
                line = line[max_width:]
                excess = len(line)
                index +=1

        self.stdscr.refresh()

    def show_message(self,message):
        if self.mode == MODE_CURSES:
            self.show_curses_message(message)
 

    def initialize_curses_board(self):
        self.stdscr = curses.initscr()
        curses.noecho()
        self.show_curses_message(self.welcome)
        self.stdscr.getkey()
        self.stdscr.clear()
            
    def initialize_board(self):
        if self.mode == MODE_CURSES:
            self.initialize_curses_board()

    def add_question(self,q_type,text,options=False,solution=False,resp=False):
        cur_question = question(q_type, text, parent=self, q_options=options, mode=self.mode,solution=solution)
        try:
            if cur_question.q_text == self.questions[self.question_index].text and \
                cur_question.q_type == self.questions[self.question_index].type:
                cur_question.q_resp = self.questions[self.question_index].resp
        except:
            pass
        if resp is not None:
            cur_question.update_resp(resp)
        self.questions.append(cur_question)
        self.question_index += 1

    def start_test(self):
        if self.mode == MODE_CURSES:
            curses.wrapper(self._wrapped_start_test)
        else:
            self._wrapped_start_test()

    def _wrapped_start_test(self, *args):
        self.initialize_board()
        for q in self.questions:
            q.ask_question()
        self.teardown()

    def teardown(self):
        if self.mode == MODE_CURSES:
            curses.endwin()
    
    def export_test(self,detail=DETAIL_WITH_RESPONSES_AND_GRADES):
        output = []
        for question in self.questions:
            output.append(question.export(detail))
        return output
        

    def __init__(self, mode=MODE_CURSES,save_dir=False):
        self.mode = mode
        self.questions = []
        self.question_index = 0
        self.save_dir = save_dir
        self.welcome = self.WELCOME_SCREEN
        if save_dir:
                self.save_dir = pathlib.Path(save_dir)
                self.save_dir.mkdir(exist_ok=True)
                self.welcome = self.WELCOME_SCREEN + "\nUsing save_dir at: {}\n".format(str(self.save_dir))


"""
Reads quiz in from yaml file, validates it, runs quiz, and saves output to new file.
"""

class quiz_builder:
    
    def import_from_file(self):
        quiz_source = self.in_file.read_text()
        self.quiz_source_obj = yaml.full_load(quiz_source)
        

    def import_solution_from_file(self,path):
        f = pathlib.Path(path)
        obj = yaml.full_load(f.read_text())
        self.import_solution(obj)

    def import_solution(self,sol):
        #n*n time, yuck
        #but i don't want to have to track IDs for questions on disk
        #uses type/text combo as the unique identifier
        for obj in sol:
            for q in self.quiz_source_obj:
                if obj["type"] == q["type"] and obj["text"] == q["text"]:
                    q["soln"] = obj["soln"]

    def validate_quiz_obj(self):
        assert isinstance(self.quiz_source_obj,list)
        for question in self.quiz_source_obj:
            assert question["type"] in Q_TYPES.keys()
            assert question["text"] is not None
            if question["type"] == Q_TYPE_KEY_MC or question["type"] == Q_TYPE_KEY_MA:
                assert question["choices"] is not None

    def build_quiz(self):
        self.quiz = quiz(save_dir=self.save_dir, mode=self.mode)

        for q in self.quiz_source_obj:
            options = []
            soln = ""
            resp = False
            if q["type"] == Q_TYPE_KEY_MC or q["type"] == Q_TYPE_KEY_MA:
                options = q["choices"]
            if "soln" in q.keys():
                soln = q["soln"]
            if "resp" in q.keys():
                resp = q["resp"]
            self.quiz.add_question(q["type"],q["text"],options,soln,resp)

    def run_quiz(self):
        self.build_quiz()
        self.quiz.start_test()
        

    def grade_quiz(self):
        self.mode = MODE_GRADE
        self.run_quiz()


    def save_results(self,detail=DETAIL_WITH_RESPONSES):
        results = self.quiz.export_test(detail=detail)
        self.out_file.write_text(yaml.dump(results))

    def save_graded_results(self):
        results = self.quiz.export_test(DETAIL_WITH_RESPONSES_AND_GRADES)
        self.out_gradefile.write_text(yaml.dump(results))

    def export_clean(self,clean=False,display=True):
        return self.export_test(clean=True)

    def export_test(self,clean=False,display=True):
        #self.build_quiz()
        if clean:
            detail = DETAIL_NO_RESPONSES
        else:
            detail = DETAIL_WITH_RESPONSES_AND_GRADES

        output = self.quiz.export_test(detail)
        if display:
            print(yaml.dump(output))
        return output


    
    def __init__(self,in_file,out_file=False, out_gradefile=False, save_dir="./.quiz_save", mode=MODE_CURSES):
        if not out_file:
            out_file = "{}.response.yaml".format(in_file)
        if not out_gradefile:
            out_gradefile = "{}.graded.response.yaml".format(in_file)

        self.in_file = pathlib.Path(in_file)
        self.out_file = pathlib.Path(out_file)
        self.out_gradefile = pathlib.Path(out_gradefile)
        self.mode = mode
        self.quiz = False

        self.save_dir = save_dir

        if not self.in_file.exists() and not self.in_file.is_file():
            raise Exception("in_file does not exist")

        self.import_from_file()
        self.validate_quiz_obj()

def test_quiz(*args):
    questions = [
        [Q_TYPE_KEY_TF, "The answer to this question is true.",[], "true"],
        [Q_TYPE_KEY_FIB, "Fill ____ the blank word in this sentence.", [], "in"],
        [Q_TYPE_KEY_MC, "The third option is the answer", ["WHAT IS YOUR NAME?", "WHAT IS YOUR QUEST?", "WHAT IS YOUR FAVORITE COLOR", "RED, NO BLUE!"], 2],
        [Q_TYPE_KEY_SA, "Any phrase that meets the regex, \".*meets.*regex.*\" will satisfy this short answer.", [], ".*meets.*regex.*"]
    ]
    q = quiz(save_dir="./.quiz_save")
    for question in questions:
        q.add_question(question[0],question[1],question[2],question[3])
    q.start_test()

    print("Results:")
    print(yaml.dump(q.export_test()))

if __name__ == "__main__":
    import os
    if ENVIRON_KEY_FILE in os.environ.keys():
        built = quiz_builder(os.environ[ENVIRON_KEY_FILE])
        built.run_quiz()
        built.save_results()

