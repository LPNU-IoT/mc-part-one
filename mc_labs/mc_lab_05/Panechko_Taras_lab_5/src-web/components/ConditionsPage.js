import React, { useEffect, useState } from "react";
import axios from "axios";
import { Card, CardContent } from "./ui/card";
import { format } from "date-fns";
import "../styles/ConditionPage.css";

const API_BASE = "http://localhost:8000/api/v1/conditions";

export default function ConditionsPage() {
    const [conditions, setConditions] = useState([]);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState("");
    const [avgTemp, setAvgTemp] = useState(null);

    useEffect(() => {
        fetchConditions();
    }, []);

    const fetchConditions = async () => {
        setLoading(true);
        setError("");
        try {
            const response = await axios.get(API_BASE, {
                params: {
                    is_last_updated_values: true,
                },
            });
            const data = response.data;
            setConditions(data);
            const avg = data.reduce((sum, c) => sum + c.temperature, 0) / data.length;
            setAvgTemp(avg);
        } catch (err) {
            setError("Failed to fetch conditions");
        } finally {
            setLoading(false);
        }
    };

    const getFontClass = () => {
        if (avgTemp === null) return "default-font";
        if (avgTemp < 0) return "cold-font";
        if (avgTemp < 15) return "cool-font";
        if (avgTemp < 30) return "warm-font";
        return "hot-font";
    };

    return (
        <div className={`page-container ${getFontClass()}`}>
            <h1>Current Weather Conditions</h1>
            {loading && <p>Loading...</p>}
            {error && <p className="error">{error}</p>}
            <div className="cards-grid">
                {conditions.map((cond) => (
                    <Card key={cond.id ?? cond.device_name}>
                        <CardContent>
                            <h2>{cond.device_name}</h2>
                            <p><strong>Location:</strong> {cond.location}</p>
                            <p><strong>Temperature:</strong> {cond.temperature} °C</p>
                            <p><strong>Humidity:</strong> {cond.humidity} %</p>
                            <p><strong>Time:</strong> {format(new Date(cond.created_at), 'yyyy-MM-dd HH:mm:ss')}</p>
                        </CardContent>
                    </Card>
                ))}
            </div>
        </div>
    );
}
