import requests_cache
from pydantic_settings import BaseSettings
from retry_requests import retry
from sqlalchemy import Engine
from sqlmodel import create_engine, Session, SQLModel
from typing import  Iterator


class Settings(BaseSettings):
    DATABASE_URL: str
    MQTT_TOPIC: str
    MQTT_HOST: str
    MQTT_PORT: int
    MQTT_KEEP_ALIVE: int
    LVIV: dict[str, float] = {"latitude": 49.8397, "longitude": 24.0297}
    CACHE_DB_NAME: str
    OPENMETEO_URL: str
    OPENMETEO_WHETHER_CONDITIONS: dict[str, str] = {"current": "temperature_2m,relative_humidity_2m"}


    def get_engine(self) -> Engine:
        return create_engine(self.DATABASE_URL)

    def get_sqlite_session(self) -> Iterator[Session]:
        with Session(self.get_engine()) as session:
            yield session

    @staticmethod
    def create_db_and_tables() -> None:
        SQLModel.metadata.create_all(settings.get_engine())

    def get_retry_cache_session(self):
        cache_session = requests_cache.CachedSession(self.CACHE_DB_NAME, expire_after=3600)
        return retry(cache_session, backoff_factor=0.1)

    class Config:
        env_file = ".env"


settings = Settings()
