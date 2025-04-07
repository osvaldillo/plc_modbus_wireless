// config.h
#ifndef CONFIG_H
#define CONFIG_H

// Declaración y definición de la variable en el mismo archivo .h
const char* html = R"(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>Secuencias Neumáticas</title>
  <style>
    * {
      box-sizing: border-box;
    }

    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f4;
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
      max-width: 100%;
      width: 100%;
      max-width: 600px;
      margin: auto;
    }

    .label-container,
    .button-container {
      display: flex;
      flex-wrap: wrap;
      justify-content: center;
      gap: 10px;
      width: 100%;
    }

    .label-item {
      display: flex;
      flex-direction: column;
      align-items: center;
      flex: 1 1 100px;
    }

    .label {
      padding: 12px;
      border-radius: 5px;
      cursor: grab;
      font-weight: bold;
      transition: transform 0.2s, background-color 0.3s;
      border: 1px solid #333;
      width: 60px;
      text-align: center;
    }

    .label:hover {
      transform: scale(1.1);
    }

    .label.plus {
      background-color: #3498db;
      color: white;
    }

    .label.minus {
      background-color: #2ecc71;
      color: white;
    }

    .drop-zone {
      width: 100%;
      min-height: 80px;
      border: 2px dashed #007bff;
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
      width: 100px;
    }

    .btn:hover {
      opacity: 0.85;
      transform: scale(1.05);
    }

    .btn:disabled {
      background-color: #ccc;
      cursor: not-allowed;
    }

    #undoBtn {
      background-color: #e74c3c;
      color: white;
    }

    #sendBtn {
      background-color: #007bff;
      color: white;
    }

    .instructions {
      font-size: 14px;
      color: #555;
      max-width: 100%;
      text-align: center;
    }

    @media (max-width: 600px) {
      .label-item {
        flex: 1 1 40%;
      }

      .btn {
        width: 80px;
        font-size: 14px;
      }

      .label {
        width: 50px;
        font-size: 14px;
      }

      .instructions {
        font-size: 13px;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>Construcción de Secuencia Neumática</h2>
    <p class="instructions">
      Arrastra los elementos a la zona de secuencia y alterna entre + y - con el botón "Alternar".
    </p>
    <div class="label-container" id="labelContainer">
      <div class="label-item">
        <button class="btn toggle-btn">Alternar</button>
        <div class="label plus" draggable="true">A+</div>
      </div>
      <div class="label-item">
        <button class="btn toggle-btn">Alternar</button>
        <div class="label plus" draggable="true">B+</div>
      </div>
      <div class="label-item">
        <button class="btn toggle-btn">Alternar</button>
        <div class="label plus" draggable="true">C+</div>
      </div>
      <div class="label-item">
        <button class="btn toggle-btn">Alternar</button>
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
            let secuencia = label.innerText;
            let url = "/?secuencia=" + encodeURIComponent(secuencia);
            fetch(url)
                .then(response => console.log("Secuencia enviada: ", secuencia))
                .catch(error => console.error("Error al enviar secuencia", error));
            
            const newText = label.innerText.endsWith('+') ? label.innerText.replace('+', '-') : label.innerText.replace('-', '+');
            label.innerText = newText;
            label.classList.toggle('plus');
            label.classList.toggle('minus');
            return newText;
        }
        function toggleLabelByDrop(label) {            
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
                toggleLabelByDrop(originalLabel);
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
                toggleLabelByDrop(lastEntry.originalLabel);
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
            const times = [
                {letra:"A", valor: a_times},
                {letra:"B", valor: b_times},
                {letra:"C", valor: c_times},
                {letra:"D", valor: d_times}
            ];
            let faltante = "";
            times.forEach(cil=>{
                if (cil.valor%2 != 0){
                    faltante += faltante ? ", " + cil.letra : cil.letra;
                }
            })
            if(faltante){
                alert("No todos los cilindros regresan a su posición inicial. Revisa tu secuencia, e inténtalo de nuevo. Cilindros: (" + faltante + ").")
            }
            else{
            let secuencia = document.getElementById("resultado").innerText;
            let url = "/?secuencia=" + encodeURIComponent(secuencia);
            fetch(url)
                .then(response => console.log("Secuencia enviada: ", secuencia))
                .catch(error => console.error("Error al enviar secuencia", error));
            };
        });
    </script>
</body>
</html>
)";

#endif

