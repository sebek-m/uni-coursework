from app import db, Settings

db.create_all()

default_settings = Settings(owner_id=None, feedback=1, constant_time_version=0, t_value=2500, trial_count=15)

db.session.add(default_settings)
db.session.commit()
