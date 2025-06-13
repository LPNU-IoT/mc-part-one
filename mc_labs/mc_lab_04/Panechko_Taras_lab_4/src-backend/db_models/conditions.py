from sqlmodel import SQLModel, Field, Relationship
from datetime import datetime, timezone

class Condition(SQLModel, table=True):
    __table_args__ = {"extend_existing": True}

    id: int | None = Field(default=None, primary_key=True)
    device_name: str
    location: str
    humidity: float = Field(ge=0, le=100, description="%")
    temperature: float
    created_at: datetime | None = Field(default_factory=lambda: datetime.now(timezone.utc))


# class Device(SQLModel, table=True):
#     id: int | None = Field(default=None, primary_key=True)
#     name: str
#     location: str
#     description: str | None = None
#     conditions: list["Condition"] = Relationship(back_populates="device")
#
#
# class Condition(SQLModel, table=True):
#     __table_args__ = {"extend_existing": True}
#
#     id: int | None = Field(default=None, primary_key=True)
#     device_id: int = Field(foreign_key="device.id")
#     humidity: float = Field(ge=0,le=100, description="%")
#     temperature: float
#     created_at: datetime | None = Field(default_factory=lambda: datetime.now(timezone.utc))
#
#     device: Device = Relationship(back_populates="conditions")
#
#
# class Alert(SQLModel, table=True):
#     id: int | None = Field(default=None, primary_key=True)
#     device_id: int = Field(foreign_key="device.id")
#     message: str
#     triggered_at: datetime = Field(default_factory=lambda: datetime.now(timezone.utc))
#     resolved: bool = Field(default=False)
#     device: Device = Relationship()
