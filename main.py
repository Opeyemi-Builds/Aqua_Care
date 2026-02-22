from fastapi import FastAPI, Depends, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import StreamingResponse
from sqlalchemy.orm import Session
import database
import models
import io
import csv
import datetime

# Creating the database tables
models.Base.metadata.create_all(bind=database.engine)

app = FastAPI(title="Aqua Care Professional API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)


def get_db():
    db = database.SessionLocal()
    try:
        yield db
    finally:
        db.close()
# ESP32 DATA ENDPOINT


@app.post("/api/sensor-data")
async def receive_data(data: dict, db: Session = Depends(get_db)):
    new_entry = models.SensorData(
        moisture=data['moisture'],
        temperature=data['temperature'],
        humidity=data['humidity'],
        water_level=data['water_level'],
        pump_status=data['pump_status']
    )
    db.add(new_entry)
    db.commit()
    return {"status": "Aqua Care Synced"}

# DASHBOARD & HISTORY ENDPOINTS


@app.get("/api/history")
def get_history(db: Session = Depends(get_db)):
    # Returns last 20 readings for the charts
    return db.query(models.SensorData).order_by(models.SensorData.id.desc()).limit(20).all()[::-1]


@app.get("/api/weather-advice")
async def get_weather():

    try:
        url = "https://api.open-meteo.com/v1/forecast?latitude=7.25&longitude=5.20&current_weather=true"
        async with httpx.AsyncClient() as client:
            res = await client.get(url)
            w = res.json()["current_weather"]
            advice = "System Stable. Sensors Normal."
            if w["weathercode"] >= 51:
                advice = "Rain Forecasted: AquaCare suggests keeping Pump OFF."
            elif w["temperature"] > 34:
                advice = "High Heat: Increased evaporation detected."
            return {"advice": advice, "temp": w["temperature"]}
    except:
        return {"advice": "Analyzing local climate...", "temp": "--"}

# FARM RECORDS & CSV EXPORT


@app.post("/api/records")
def add_record(record: dict, db: Session = Depends(get_db)):
    new_rec = models.FarmRecord(
        note=record['note'], category=record['category'])
    db.add(new_rec)
    db.commit()
    return {"status": "Record Saved"}


@app.get("/api/records/all")
def get_records(db: Session = Depends(get_db)):
    return db.query(models.FarmRecord).order_by(models.FarmRecord.id.desc()).all()


@app.get("/api/records/download")
def download_records(db: Session = Depends(get_db)):
    sensors = db.query(models.SensorData).all()
    logs = db.query(models.FarmRecord).all()

    output = io.StringIO()
    writer = csv.writer(output)
    writer.writerow(["AQUA CARE - MASTER FARM AUDIT"])
    writer.writerow(["Generated:", datetime.datetime.now()])
    writer.writerow([])
    writer.writerow(["--- SENSOR HISTORY ---"])
    writer.writerow(["Time", "Moisture%", "Temp C",
                    "Humidity%", "Tank%", "Pump"])
    for s in sensors:
        writer.writerow([s.timestamp, s.moisture, s.temperature,
                        s.humidity, s.water_level, s.pump_status])

    writer.writerow([])
    writer.writerow(["--- MANUAL LOGS ---"])
    writer.writerow(["Time", "Category", "Note"])
    for l in logs:
        writer.writerow([l.timestamp, l.category, l.note])

    output.seek(0)
    return StreamingResponse(output, media_type="text/csv", headers={"Content-Disposition": "attachment; filename=AquaCare_Report.csv"})
