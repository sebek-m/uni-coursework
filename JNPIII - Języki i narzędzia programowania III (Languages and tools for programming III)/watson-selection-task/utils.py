import os
import json

base_dir = os.path.dirname(__file__)

def get_json(filename):
  try:
    with open(filename) as json_file:
      return json.load(json_file)
  except FileNotFoundError:
    return None

def get_task_json(task_num):
  return get_json(f'{base_dir}/tasks/{task_num}.json')

def get_settings_json():
  return get_json(f'{base_dir}/settings.json')

def update_json(filename, json_data):
  with open(filename, 'w', encoding='utf8') as out_json_file:
    json.dump(json_data, out_json_file, indent=2, ensure_ascii=False)

def update_task_json(task_num, task_settings):
  update_json(f'{base_dir}/tasks/{task_num}.json', task_settings)

def update_settings_json(new_settings):
  update_json(f'{base_dir}/settings.json', new_settings)