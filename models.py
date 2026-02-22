from sqlalchemy import Column, Integer, Float, Boolean, String, DateTime
from database import Base
import datetime

class SensorData(Base):
    __tablename__ = "sensor_data"
    id = Column(Integer, primary_key=True, index=True)
    moisture = Column(Integer)
    temperature = Column(Float)
    humidity = Column(Float)
    water_level = Column(Float)
    pump_status = Column(Boolean)
    timestamp = Column(DateTime, default=datetime.datetime.utcnow)

class FarmRecord(Base):
    __tablename__ = "farm_records"
    id = Column(Integer, primary_key=True, index=True)
    note = Column(String)
    category = Column(String, default="General")
    timestamp = Column(DateTime, default=datetime.datetime.utcnow)