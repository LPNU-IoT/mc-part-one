import React from "react";
import "../../styles/card.css"

export function Card({ children }) {
    return (
        <div className="card">
            {children}
        </div>
    );
}

export function CardContent({ children }) {
    return (
        <div className="card-content">
            {children}
        </div>
    );
}
