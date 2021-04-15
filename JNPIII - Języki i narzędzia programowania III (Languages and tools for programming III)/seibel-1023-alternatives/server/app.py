from flask import Flask, render_template, request, abort, Response
from datetime import datetime
from flask_sqlalchemy import SQLAlchemy
from flask_csv import send_csv
from copy import copy
app = Flask(__name__, static_url_path='/static')

app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///experiment_db.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

class Participant(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    nickname = db.Column(db.String, unique=True, nullable=False)
    age = db.Column(db.Integer, nullable=False)
    sex = db.Column(db.String, nullable=False)

    settings = db.relationship('Settings', backref="owner")
    sessions = db.relationship('ExperimentSession', backref="subject")

    def as_dict(self):
       return {c.name: getattr(self, c.name) for c in self.__table__.columns}

    def __repr__(self):
        return '<Participant %r>' % self.nickname

class Settings(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    owner_id = db.Column(db.Integer, db.ForeignKey(Participant.id), nullable=True)
    feedback = db.Column(db.Integer, nullable=False)
    constant_time_version = db.Column(db.Integer, nullable=False)
    trial_count = db.Column(db.Integer, nullable=False)
    t_value = db.Column(db.Integer, nullable=False)
    
class ExperimentSession(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    subject_id = db.Column(db.Integer, db.ForeignKey(Participant.id), nullable=False)
    session_date = db.Column(db.DateTime, nullable=False)
    start = db.Column(db.Integer, nullable=False)
    end = db.Column(db.Integer, nullable=False)
    duration = db.Column(db.Integer, nullable=False)
    feedback = db.Column(db.Integer, nullable=False)
    constant_time_version = db.Column(db.Integer, nullable=False)
    trial_count = db.Column(db.Integer, nullable=False)
    t_value = db.Column(db.Integer, nullable=False)

    patterns = db.relationship('Pattern', backref="session")

    def as_dict(self):
      return {c.name: getattr(self, c.name) for c in self.__table__.columns}

class Pattern(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    session_id = db.Column(db.Integer, db.ForeignKey(ExperimentSession.id), nullable=False)
    moment = db.Column(db.Integer, nullable=False)
    pattern = db.Column(db.Integer, nullable=False)

    answers = db.relationship('Answer', backref="pattern")

    def as_dict(self):
      return {c.name: getattr(self, c.name) for c in self.__table__.columns}

class Answer(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    pattern_id = db.Column(db.Integer, db.ForeignKey(Pattern.id), nullable=False)
    lamp_num = db.Column(db.Integer, nullable=False)
    answer_time = db.Column(db.Integer, nullable=False)
    correct = db.Column(db.Integer, nullable=False)

    def as_dict(self):
      return {c.name: getattr(self, c.name) for c in self.__table__.columns}

@app.route('/')
def main_page():
    return render_template('index.html')

@app.route('/login', methods=["POST"])
def login():
    nickname_json = request.get_json()
    nickname = nickname_json["nickname"]

    if nickname is None or nickname == '':
        abort(400)

    participant = Participant.query.filter(Participant.nickname == str(nickname)).one_or_none()

    if participant is None:
      return {
        "info": "no such user"
      }

    participant_settings = Settings.query.filter(Settings.owner_id == participant.id).first()
    participant_sessions = ExperimentSession.query.filter(ExperimentSession.subject_id == participant.id) \
                                                  .filter(ExperimentSession.feedback == participant_settings.feedback) \
                                                  .filter(ExperimentSession.constant_time_version == participant_settings.constant_time_version) \
                                                  .all()

    shown_patterns = []

    for session in participant_sessions:
      patterns = [pattern.pattern for pattern in session.patterns]
      shown_patterns.extend(patterns)

    return {
      "id": participant.id,
      "finished_patterns": shown_patterns,
      "feedback_loop": (True if participant_settings.feedback == 1 else False),
      "constant_time_version": (True if participant_settings.constant_time_version == 1 else False),
      "t_value": participant_settings.t_value,
      "trial_count": participant_settings.trial_count
    }

@app.route('/register-participant', methods=["POST"])
def register():
  participant_info = request.get_json()

  new_participant = Participant(nickname=participant_info["nickname"], age=participant_info["age"], sex=participant_info["sex"])
  db.session.add(new_participant)
  db.session.commit()

  default_settings = Settings.query.filter(Settings.owner_id == None).first()

  participant_settings = Settings(owner_id=new_participant.id, feedback=default_settings.feedback, \
                                  constant_time_version=default_settings.constant_time_version, \
                                  t_value=default_settings.t_value, trial_count=default_settings.trial_count)
  db.session.add(participant_settings)
  db.session.commit()

  return {
    "id": new_participant.id,
    "finished_patterns": [],
    "feedback_loop": (True if participant_settings.feedback == 1 else False),
    "constant_time_version": (True if participant_settings.constant_time_version == 1 else False),
    "t_value": participant_settings.t_value,
    "trial_count": participant_settings.trial_count
  }

@app.route('/save-default-settings', methods=["POST"])
def save_default_settings():
  new_settings = request.get_json()

  default_settings = Settings.query.filter(Settings.owner_id == None).first()

  if new_settings["feedback_loop"] is not None:
    default_settings.feedback = (1 if new_settings["feedback_loop"] else 0)

  if new_settings["constant_time_version"] is not None:
    default_settings.constant_time_version = (1 if new_settings["constant_time_version"] else 0)

  if new_settings["t_value"] is not None:
    default_settings.t_value = new_settings["t_value"]

  if new_settings["trial_count"] is not None:
    default_settings.trial_count = new_settings["trial_count"]

  db.session.commit()

  return Response(status=200)

@app.route('/save-participant-settings', methods=["POST"])
def save_participant_settings():
  new_settings = request.get_json()

  settings = Settings.query.filter(Settings.owner_id == new_settings["id"]).first()

  if new_settings["feedback_loop"] is not None:
    settings.feedback = (1 if new_settings["feedback_loop"] else 0)

  if new_settings["constant_time_version"] is not None:
    settings.constant_time_version = (1 if new_settings["constant_time_version"] else 0)

  if new_settings["t_value"] is not None:
    settings.t_value = new_settings["t_value"]

  if new_settings["trial_count"] is not None:
    settings.trial_count = new_settings["trial_count"]

  db.session.commit()

  participant_settings = Settings.query.filter(Settings.owner_id == new_settings["id"]).first()
  participant_sessions = ExperimentSession.query.filter(ExperimentSession.subject_id == new_settings["id"]) \
                                                .filter(ExperimentSession.feedback == participant_settings.feedback) \
                                                .filter(ExperimentSession.constant_time_version == participant_settings.constant_time_version) \
                                                .all()

  shown_patterns = []

  for session in participant_sessions:
    patterns = [pattern.pattern for pattern in session.patterns]
    shown_patterns.extend(patterns)

  return {
    "id": new_settings["id"],
    "finished_patterns": shown_patterns,
    "feedback_loop": (True if settings.feedback == 1 else False),
    "constant_time_version": (True if settings.constant_time_version == 1 else False),
    "t_value": settings.t_value,
    "trial_count": settings.trial_count
  }

@app.route('/begin-session', methods=["POST"])
def begin_session():
  session_info = request.get_json()

  session_row = ExperimentSession(subject_id=session_info["id"], session_date=datetime.now(), \
                                  start=session_info["start"], end=session_info["start"], \
                                  duration=0, feedback=session_info["feedback_loop"], \
                                  constant_time_version=session_info["constant_time_version"], \
                                  t_value=session_info["t_value"], trial_count=session_info["trial_count"])

  db.session.add(session_row)
  db.session.commit()

  return {
    "session_id": session_row.id
  }

@app.route('/submit-pattern-answers', methods=["POST"])
def submit_answers():
  pattern_answers = request.get_json()

  pattern_row = Pattern(session_id=pattern_answers["session_id"], moment=pattern_answers["start"], pattern=pattern_answers["pattern"])
  parent_session = ExperimentSession.query.filter(ExperimentSession.id == pattern_answers["session_id"]).first()
  parent_session.end = pattern_answers["end"]
  parent_session.duration = parent_session.end - parent_session.start
  db.session.add(pattern_row)
  db.session.commit()

  for i, answer in enumerate(pattern_answers["answers"]):
    if answer is not None:
      answer_row = Answer(pattern_id=pattern_row.id, lamp_num=i, answer_time=(answer["time"] - pattern_row.moment), correct=(1 if answer["correct"] else 0))
      db.session.add(answer_row)

  db.session.commit()

  return Response(status=200)

@app.route('/api/subject-data/<string:nickname>')
def provide_data(nickname):
  if nickname is None or nickname == "":
    return {
      "error": "invalid nickname"
    }

  participant = Participant.query.filter(Participant.nickname == nickname).first()

  if participant is None:
    return {
      "info": "no such subject"
    }

  sessions = ExperimentSession.query.filter(ExperimentSession.subject_id == participant.id).order_by(ExperimentSession.start).all()
  full_data_sessions = []

  for session in sessions:
    patterns_in_session = Pattern.query.filter(Pattern.session_id == session.id).order_by(Pattern.moment).all()
    patterns_with_answers = []

    for pattern in patterns_in_session:
      answers = Answer.query.filter(Answer.pattern_id == pattern.id).order_by(Answer.lamp_num).all()
      answers = [answer.as_dict() for answer in answers]
      patterns_with_answers.append({
        "pattern_info": pattern.as_dict(),
        "answers": answers
      })

    full_data_sessions.append({
      "session_info": session.as_dict(),
      "patterns_shown": patterns_with_answers
    })
  
  return {
    "subject_info": participant.as_dict(),
    "sessions": full_data_sessions
  }

column_names = ["Session ID", "Session Date", "Time t", "Planned trial count", \
                "Feedback Loop mode", "Overall Time version", "Session Start", \
                "Session End", "Session Duration", \
                "Pattern", "Pattern Source Number", "Pattern Start"]

for i in range(10):
  column_names.append(f'Lamp {i} Answer Time')
  column_names.append(f'Lamp {i} Correct')

@app.route('/api/csv/subject-data/<string:nickname>')
def provide_csv_data(nickname):
  if nickname is None or nickname == "":
    return {
      "error": "invalid nickname"
    }

  participant = Participant.query.filter(Participant.nickname == nickname).first()

  if participant is None:
    return {
      "info": "no such subject"
    }

  current_pattern = dict.fromkeys(column_names, '-')

  rows = []

  sessions = ExperimentSession.query.filter(ExperimentSession.subject_id == participant.id).order_by(ExperimentSession.start).all()

  for session in sessions:
    current_pattern["Session ID"] = session.id
    current_pattern["Session Date"] = session.session_date
    current_pattern["Time t"] = session.t_value
    current_pattern["Planned trial count"] = session.trial_count
    current_pattern["Feedback Loop mode"] = session.feedback
    current_pattern["Overall Time version"] = session.constant_time_version
    current_pattern["Session Start"] = session.start
    current_pattern["Session End"] = session.end
    current_pattern["Session Duration"] = session.duration

    patterns_in_session = Pattern.query.filter(Pattern.session_id == session.id).order_by(Pattern.moment).all()

    for pattern in patterns_in_session:
      current_pattern["Pattern"] = f'{pattern.pattern:010b}'
      current_pattern["Pattern Source Number"] = pattern.pattern
      current_pattern["Pattern Start"] = pattern.moment

      answers = Answer.query.filter(Answer.pattern_id == pattern.id).order_by(Answer.lamp_num).all()
      for answer in answers:
        index = f'Lamp {answer.lamp_num}'

        current_pattern[f'{index} Answer Time'] = answer.answer_time
        current_pattern[f'{index} Correct'] = answer.correct

      rows.append(copy(current_pattern))

      for i in range(10):
        current_pattern[f'Lamp {i} Answer Time'] = '-'
        current_pattern[f'Lamp {i} Correct'] = '-'

  return send_csv(rows, \
                  f'{nickname}_{participant.age}_{participant.sex}.csv', \
                  column_names)