async function getSettings() {
  let response = await fetch('/api/settings');
  if (!response.ok)
    throw Error('Nie udało się pobrać ustawień');

  return await response.json();
}

async function getTask(taskNum) {
  let response = await fetch(`/api/tasks/${taskNum}`);
  if (!response.ok)
    throw Error('Nie udało się pobrać zadania');

  return await response.json();
}

async function sendData(data) {
  let response = await fetch('/save-data', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: JSON.stringify(data)
  });

  if (!response.ok) {
    alert("Nie udało się zapisać danych.");
  }
}

export { getSettings, getTask, sendData }