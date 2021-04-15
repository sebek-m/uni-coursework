from flask import Flask, render_template, request, abort, Response, redirect
from utils import update_json, get_settings_json, base_dir, get_json, update_settings_json, update_task_json, get_task_json
import os
from flask_csv import send_csv
from copy import copy

app = Flask(__name__, static_url_path='/static')
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0

def scan_tasks():
  settings = get_settings_json()

  old_task_list = [(task['number'], task['selected']) for task in settings['stage_four']['tasks']]
  old_tasks = dict(old_task_list)

  old_pilot_task_list = [(task['number'], task['selected']) for task in settings['pilot_mode']['tasks']]
  old_pilot_tasks = dict(old_pilot_task_list)

  tasks = []
  pilot_tasks = []
  for entry in os.scandir(f'{base_dir}/tasks'):
    if (entry.path.endswith(".json") and entry.is_file()):
      task = get_json(entry.path)
      tasks.append({
        "number": task['task_number'],
        "name": task['task_name'],
        "selected": (old_tasks[task['task_number']] if task['task_number'] in old_tasks else False)
      })
      pilot_tasks.append({
        "number": task['task_number'],
        "name": task['task_name'],
        "selected": (old_pilot_tasks[task['task_number']] if task['task_number'] in old_pilot_tasks else False)
      })

  settings['stage_four']['tasks'] = tasks
  settings['pilot_mode']['tasks'] = pilot_tasks

  update_settings_json(settings)

scan_tasks()

# Settings routes

@app.route('/settings')
def settings_main():
  return render_template('settings-main.html')

@app.route('/settings/modes')
def settings_modes():
  settings = get_settings_json()
  return render_template('settings-modes.html', mode=settings['mode'])

@app.route('/set-mode/<int:mode>')
def set_mode(mode):
  if (mode < 1 or mode > 3):
    return 404

  settings = get_settings_json()
  settings['mode'] = mode
  update_settings_json(settings)

  return redirect('/settings')

@app.route('/settings/stages')
def settings_stages():
  return render_template('settings-stages.html')

def natural_stage_names(stage):
  stage_natural_name = ""
  stage_natural_number = ""
  if stage == 1:
    stage_natural_name = "Etap pierwszy"
    stage_natural_number = "stage_one"
  elif stage == 2:
    stage_natural_name = "Etap drugi"
    stage_natural_number = "stage_two"
  else:
    stage_natural_name = "Etap trzeci"
    stage_natural_number = "stage_three"

  return stage_natural_name, stage_natural_number

@app.route('/settings/stages/<int:stage>')
def resolve_stage_request(stage):
  settings = get_settings_json()

  if (stage == 4):
    return render_template('settings-experimental-stage.html', stage=settings['stage_four'])
  elif (1 <= stage and stage <= 3):
    stage_natural_name, stage_natural_number = natural_stage_names(stage)
    
    return render_template('settings-intro-stage.html', stage_num=stage, stage_name=stage_natural_name, stage=settings[stage_natural_number])
  else:
    return 404

@app.route('/settings/general')
def settings_general():
  settings = get_settings_json()
  return render_template('settings-general.html', general=settings['general'])

def submit_settings(new_settings, fieldname):
  settings = get_settings_json()
  settings[fieldname] = new_settings
  update_settings_json(settings)

@app.route('/submit-general-settings', methods=['POST'])
def submit_general_settings():
  general = request.get_json()

  submit_settings(general, 'general')

  return { "status": "ok" }, 200

@app.route('/settings/tasks')
def settings_tasks():
  settings = get_settings_json()
  return render_template('settings-tasks.html', tasks=settings['stage_four']['tasks'])

@app.route('/settings/tasks/<int:num>')
def edit_task(num):
  task_settings = get_task_json(num)

  return render_template('settings-edit-task.html', task=task_settings)

@app.route('/settings/tasks/new-task')
def new_task():
  new_task_obj = {
    "task_number": "",
    "task_name": "",
    "intro_story": "",
    "rule": "",
    "p": "",
    "non_p": "",
    "q": "",
    "non_q": "",
    "extra_description": "",
    "card_selection_task": ""
  }
  return render_template('settings-edit-task.html', task = new_task_obj)

def update_single_task_list(settings, field, task_settings):
  noted_tasks = settings['stage_four']['tasks']

  i = 0
  found = False
  while i < len(noted_tasks) and not found:
    if noted_tasks[i]['number'] == task_settings['task_number']:
      found = True
      noted_tasks[i]['name'] = task_settings['task_name']
    
    i = i + 1
  
  if not found:
    noted_tasks.append({
      "number": task_settings['task_number'],
      "name": task_settings['task_name'],
      "selected": False
    })

def update_task_lists(task_settings):
  settings = get_settings_json()
  update_single_task_list(settings, 'task_four', task_settings)
  update_single_task_list(settings, 'pilot_mode', task_settings)

  update_settings_json(settings)

@app.route('/update-task/<int:num>', methods=['POST'])
def update_task(num):
  task_settings = request.get_json()
  update_task_json(num, task_settings)

  update_task_lists(task_settings)

  return { "status": "ok" }, 200

@app.route('/settings/pilot')
def settings_pilot():
  settings = get_settings_json()
  return render_template('settings-pilot.html', stage=settings['pilot_mode'])

@app.route('/submit-pilot-settings', methods=['POST'])
def submit_pilot_settings():
  pilot_settings = request.get_json()

  submit_settings(pilot_settings, 'pilot_mode')

  return { "status": "ok" }, 200

@app.route('/submit-experimental-settings', methods=['POST'])
def submit_experimental_settings():
  experimental_settings = request.get_json()

  submit_settings(experimental_settings, 'stage_four')

  return { "status": "ok" }, 200

@app.route('/submit-stage-settings/<int:stage>', methods=['POST'])
def submit_stage_settings(stage):
  stage_settings = request.get_json()

  stage_natural_number = natural_stage_names(stage)[1]
  submit_settings(stage_settings, stage_natural_number)

  return { "status": "ok" }, 200

@app.route('/api/settings')
def api_settings():
  return get_settings_json()

@app.route('/api/experimental-settings')
def api_experimental_settings():
  settings = get_settings_json()
  return settings['stage_four']

@app.route('/api/pilot-settings')
def api_pilot_settings():
  settings = get_settings_json()
  return settings['pilot_mode']

@app.route('/api/tasks/<int:num>')
def api_tasks(num):
  return get_task_json(num)

# Main route

@app.route('/')
def main_route():
  settings = get_settings_json()
  return render_template('index.html', welcome_text=settings['general']['welcome_text'], \
                                      main_script='../static/main.js')

@app.route('/pilot')
def pilot_route():
  settings = get_settings_json()
  return render_template('index.html', welcome_text=settings['general']['welcome_text'], \
                                      main_script='../static/pilot.js')

# CSV route

@app.route('/save-data', methods=['POST'])
def save_data():
  data = request.get_json()
  update_json(f'{base_dir}/tmp/recent.json', data)

  return { "status": "ok" }, 200

column_names = ["External Task Number", "Internal Task Number", "Task Name", "GIL Clicks", \
                "Correct", "Card Order", "Task Execution Time", "Task Solution Time", \
                "P Selection Times", "nP Selection Times", \
                "Q Selection Times", "nQ Selection Times", \
                "P Deselection Times", "nP Deselection Times", \
                "Q Deselection Times", "nQ Deselection Times", \
                "Final Selection", "Final Selection Times"]

@app.route('/download-recent-csv')
def download_recent_csv():
  data = get_json(f'{base_dir}/tmp/recent.json')
  if data is None:
    return "Nie odbyto żadnej sesji."

  print(data)

  rows = []

  if data['mode'] != 'pilot':
    # Stage two data
    stage_two_row = dict.fromkeys(column_names, 'n/a')
    stage_two_row["External Task Number"] = "STAGE TWO"
    stage_two_row["Internal Task Number"] = "STAGE TWO"
    stage_two_row["GIL Clicks"] = data["stageTwoGILClicks"]

    rows.append(stage_two_row)

  # Stage four tasks data
  for task_data in data["taskData"]:
    task_row = dict.fromkeys(column_names, '-')

    task_row["External Task Number"] = task_data["externalTaskNum"]
    task_row["Internal Task Number"] = task_data["internalTaskNum"]
    task_row["Task Name"] = task_data["taskName"]
    task_row["GIL Clicks"] = task_data["GILClicks"]
    task_row["Correct"] = 1 if task_data["correct"] else 0
    task_row["Card Order"] = task_data["cardOrder"]
    task_row["Task Execution Time"] = task_data["executionDuration"]
    task_row["Task Solution Time"] = task_data["solutionDuration"]
    task_row["P Selection Times"] = task_data["selectionTimes"][0]
    task_row["nP Selection Times"] = task_data["selectionTimes"][1]
    task_row["Q Selection Times"] = task_data["selectionTimes"][2]
    task_row["nQ Selection Times"] = task_data["selectionTimes"][3]
    task_row["P Deselection Times"] = task_data["deselectionTimes"][0]
    task_row["nP Deselection Times"] = task_data["deselectionTimes"][1]
    task_row["Q Deselection Times"] = task_data["deselectionTimes"][2]
    task_row["nQ Deselection Times"] = task_data["deselectionTimes"][3]
    task_row["Final Selection"] = task_data["finalSelectionOrder"]
    task_row["Final Selection Times"] = task_data["finalSelectionOrderedTimes"]

    rows.append(task_row)

  # Sending csv
  return send_csv(rows, \
                  f'{data["nickname"]}_{data["gender"]}_{data["age"]}_mode{data["mode"]}.csv', \
                  column_names)