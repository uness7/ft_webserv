export class HandleForm {

    fields = {
        firstName: {
            value: null,
            regex: /^[\sa-zA-Z]{2,}$/,
            errorText: "Le champ prénom doit contenir au moins 2 lettres et aucun chiffre."
        },
        lastName: {
            value: null,
            regex: /^[\sa-zA-Z]{2,}$/,
            errorText: "Le champ nom doit contenir au moins 2 lettres et aucun chiffre."
        },
        userEmail: {
            value: null,
            regex: /^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$/,
            errorText: "Veuillez entrer une adresse email valide."
        },
        messageContent: {
            value: null,
            regex: null,
            errorText: "Votre message doit contenir minimum 10 caractères."
        }
    }

    constructor(formId, displayValidation) {
        this.form = document.getElementById(formId)

        Object.keys(this.fields).forEach(field => this.onChange(field))


        this.form.addEventListener('submit', (e) => {
            e.preventDefault()

            let canSubmit = true

            Object.keys(this.fields).forEach(key => {

                if (this.fields[key].value === null) {
                    canSubmit = false
                    const element = this.form.querySelector(`#${key}`).parentNode
                    this.addError(key, element)
                }
            })

            if (canSubmit) {
                displayValidation()
                Object.keys(this.fields).forEach(key => {
                    console.log({ [key]: this.fields[key].value })
                })
            }

        })
    }

    onChange(field) {
        this.form.querySelector(`#${field}`)
            .addEventListener('change', (e) => {
                if (this.fields[field].regex) {
                    this.checkInputByRegex(e, this.fields[field].regex)
                } else {
                    this.checkTextarea(e)
                }
            })
    }

    checkTextarea(e) {
        const { value, id, parentNode: parent } = e.target
        this.removeAttribute(parent)

        if (value.length >= 10) {
            this.fields[id].value = value
        } else {
            this.addError(id, parent)
        }
    }

    checkInputByRegex(e, reg) {
        const { value, id, parentNode: parent } = e.target
        this.removeAttribute(parent)

        if (reg.test(value)) {
            this.fields[id].value = value
        } else {
            this.addError(id, parent)
        }
    }

    removeAttribute(element) {
        element.removeAttribute('data-error-visible')
        element.removeAttribute('data-error')
    }

    addError(id, element) {
        this.fields[id].value = null
        element.setAttribute('data-error-visible', true)
        element.setAttribute('data-error', this.fields[id].errorText)
    }
}