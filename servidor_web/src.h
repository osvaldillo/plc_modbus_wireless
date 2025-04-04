// config.h
#ifndef CONFIG_H
#define CONFIG_H

// Declaración y definición de la variable en el mismo archivo .h
const char* html = R"(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Secuencias Neumáticas</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            margin: 0;
            padding: 20px;
        }
        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 15px;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0px 4px 6px rgba(0, 0, 0, 0.1);
            width: 90%;
            max-width: 600px;
            margin: auto;
        }
        .label-container, .button-container {
            display: flex;
            gap: 15px;
            flex-wrap: wrap;
            justify-content: center;
        }
        .label-item {
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        .label {
            padding: 12px;
            border-radius: 5px;
            cursor: grab;
            font-weight: bold;
            transition: transform 0.2s, background-color 0.3s;
            border: 1px solid #333;
        }
        .label:hover {
            transform: scale(1.1);
        }
        .label.plus { background-color: #3498db; color: white; }
        .label.minus { background-color: #2ecc71; color: white; }
        .drop-zone {
            width: 80%;
            min-height: 80px;
            border: 2px dashed #007BFF;
            background: #e3f2fd;
            padding: 10px;
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            align-items: center;
            border-radius: 5px;
            transition: background-color 0.3s;
        }
        .drop-zone:hover {
            background-color: #d0ebff;
        }
        .btn {
            padding: 10px 15px;
            border: none;
            border-radius: 5px;
            font-size: 16px;
            cursor: pointer;
            transition: background 0.3s, transform 0.2s;
        }
        .btn:hover {
            opacity: 0.8;
            transform: scale(1.05);
        }
        .btn:disabled {
            background-color: #ccc;
            cursor: not-allowed;
        }
        #undoBtn { background-color: #e74c3c; color: white; }
        #sendBtn { background-color: #007BFF; color: white; }
        .instructions {
            font-size: 14px;
            color: #555;
            max-width: 80%;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Construcción de Secuencia Neumática</h2>
        <p class="instructions">Arrastra los elementos a la zona de secuencia y alterna entre + y - con el botón "Toggle".</p>
        <div class="label-container" id="labelContainer">
            <div class="label-item">
                <button class="btn toggle-btn">Toggle</button>
                <div class="label plus" draggable="true">A+</div>
            </div>
            <div class="label-item">
                <button class="btn toggle-btn">Toggle</button>
                <div class="label plus" draggable="true">B+</div>
            </div>
            <div class="label-item">
                <button class="btn toggle-btn">Toggle</button>
                <div class="label plus" draggable="true">C+</div>
            </div>
            <div class="label-item">
                <button class="btn toggle-btn">Toggle</button>
                <div class="label plus" draggable="true">D+</div>
            </div>
        </div>
        <div class="drop-zone" id="dropZone"></div>
        <p>Secuencia: <span id="resultado"></span></p>
        <div class="button-container">
            <button class="btn" id="undoBtn">Deshacer</button>
            <button class="btn" id="sendBtn">Enviar</button>
        </div>
    </div>
    <script>
        const labelItems = document.querySelectorAll('.label-item');
        const labels = labelContainer.querySelectorAll('.label');
        const dropZone = document.getElementById('dropZone');
        const resultado = document.getElementById('resultado');
        const undoBtn = document.getElementById('undoBtn');
        const sendBtn = document.getElementById('sendBtn');
        let a_times = 0;
        let b_times = 0;
        let c_times = 0;
        let d_times = 0;
        const history = [];

        function toggleLabel(label) {
            const newText = label.innerText.endsWith('+') ? label.innerText.replace('+', '-') : label.innerText.replace('-', '+');
            label.innerText = newText;
            label.classList.toggle('plus');
            label.classList.toggle('minus');
            return newText;
        }

        labelItems.forEach(item => {
            const label = item.querySelector('.label');
            const toggleBtn = item.querySelector('.toggle-btn');

            toggleBtn.addEventListener('click', () => {
                toggleLabel(label);
            });

            label.addEventListener('dragstart', (e) => {
                e.dataTransfer.setData('text', label.innerText);
            });
        });

        dropZone.addEventListener('dragover', (e) => {
            e.preventDefault();
        });

        dropZone.addEventListener('drop', (e) => {
            e.preventDefault();
            const text = e.dataTransfer.getData('text');
            
            const newLabel = document.createElement('div');
            newLabel.classList.add('label', text.endsWith('+') ? 'plus' : 'minus');
            if(text.startsWith('A')){a_times++;}
            else if(text.startsWith('B')){b_times++;}
            else if(text.startsWith('C')){c_times++;}
            else if(text.startsWith('D')){d_times++;}
            newLabel.innerText = text;
            dropZone.appendChild(newLabel);
            history.push({ label: newLabel, originalText: text });

            resultado.textContent += resultado.textContent ? ' ' + text : text;

            labelItems.forEach(item => {
                const label = item.querySelector('.label');
                const toggleBtn = item.querySelector('.toggle-btn');
                if (label.innerText === text) {
                    toggleBtn.disabled = true;
                }
            });
            const originalLabel = [...labels].find(l => l.innerText === text);
            if (originalLabel) {
                history[history.length - 1].originalLabel = originalLabel;
                toggleLabel(originalLabel);
            }
        });

        undoBtn.addEventListener('click', () => {
            if (history.length > 0) {
                const lastEntry = history.pop();
                
                dropZone.removeChild(lastEntry.label);
                console.log(lastEntry.label.textContent);
                if(lastEntry.label.textContent.startsWith('A')){a_times--;}
                else if(lastEntry.label.textContent.startsWith('B')){b_times--;}
                else if(lastEntry.label.textContent.startsWith('C')){c_times--;}
                else if(lastEntry.label.textContent.startsWith('D')){d_times--;}
                resultado.textContent = resultado.textContent.split(' ').slice(0, -1).join(' ');
                toggleLabel(lastEntry.originalLabel);
                labelItems.forEach(item => {
                    const label = item.querySelector('.label');
                    const toggleBtn = item.querySelector('.toggle-btn');
                    if (label.innerText === lastEntry.originalText) {
                        if(label.textContent.startsWith('A') && a_times === 0){
                            toggleBtn.disabled = false;}
                        else if(label.textContent.startsWith('B') && b_times === 0){
                            toggleBtn.disabled = false;}
                        else if(label.textContent.startsWith('C') && c_times === 0){
                            toggleBtn.disabled = false;}
                        else if(label.textContent.startsWith('D') && d_times === 0){
                            toggleBtn.disabled = false;}
                    }
                });
            }
        });
        
        sendBtn.addEventListener('click', () => {
            let secuencia = document.getElementById("resultado").innerText;
            let url = "/?secuencia=" + encodeURIComponent(secuencia);
            fetch(url)
                .then(response => console.log("Secuencia enviada: ", secuencia))
                .catch(error => console.error("Error al enviar secuencia", error));
        });
    </script>
</body>
</html>


)";

#endif

