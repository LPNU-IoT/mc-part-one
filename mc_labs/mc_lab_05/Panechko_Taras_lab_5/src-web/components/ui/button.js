import React from "react";
import "../../styles/button.css"

export function Button({ children, onClick }) {
    return (
        <button className="green-button" onClick={onClick}>
            {children}
        </button>
    );
}
