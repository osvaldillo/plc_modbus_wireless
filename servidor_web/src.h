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
        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 10px;
            margin-top: 50px;
        }
        .label-container, .button-container {
            display: flex;
            gap: 10px;
        }
        .label {
            padding: 10px;
            border: 1px solid #000;
            cursor: grab;
        }
        .label.plus { background-color: lightblue; }
        .label.minus { background-color: lightgreen; }
        .drop-zone {
            width: 300px;
            min-height: 50px;
            border: 2px dashed #000;
            padding: 10px;
            display: flex;
            flex-wrap: wrap;
            gap: 5px;
        }
        .btn {
            padding: 8px 12px;
            border: none;
            background-color: #007BFF;
            color: white;
            cursor: pointer;
        }
        .btn:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="label-container" id="labelContainer">
            <div class="label plus" draggable="true">A+</div>
            <div class="label plus" draggable="true">B+</div>
            <div class="label plus" draggable="true">C+</div>
            <div class="label plus" draggable="true">D+</div>
        </div>
        <div class="drop-zone" id="dropZone"></div>
        <p>Secuencia: <span id="resultado"></span></p>
        <div class="button-container">
            <button class="btn" id="undoBtn">Deshacer</button>
            <button class="btn" id="sendBtn">Enviar</button>
        </div>
    </div>
    <script>
        const labelContainer = document.getElementById('labelContainer');
        const labels = labelContainer.querySelectorAll('.label');
        const dropZone = document.getElementById('dropZone');
        const resultado = document.getElementById('resultado');
        const undoBtn = document.getElementById('undoBtn');
        const sendBtn = document.getElementById('sendBtn');
        const history = [];

        function toggleLabel(label) {
            const newText = label.innerText.endsWith('+') ? label.innerText.replace('+', '-') : label.innerText.replace('-', '+');
            label.innerText = newText;
            label.classList.toggle('plus');
            label.classList.toggle('minus');
            return newText;
        }

        labels.forEach(label => {
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
            newLabel.innerText = text;
            dropZone.appendChild(newLabel);
            history.push({ label: newLabel, originalText: text });

            resultado.textContent += resultado.textContent ? ' ' + text : text;
            
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
                resultado.textContent = resultado.textContent.split(' ').slice(0, -1).join(' ');
                if (lastEntry.originalLabel) {
                    toggleLabel(lastEntry.originalLabel);
                }
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

